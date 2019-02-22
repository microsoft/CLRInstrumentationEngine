#include "stdafx.h"
#include "managed_function.h"

using namespace std;

namespace vanguard {
    namespace instrumentation {
        namespace managed
        {
            //! Constructs a function given the symbol and metadata.
            function::function() :
                _id(INVALID_ID),
                _rva(0),
                _address(nullptr),
                _token(0),
                _type_token(0),
                _size(0),
                _name(nullptr),
                _type_name(nullptr),
                _namespace_name(nullptr),
                _source_file(nullptr),
                _blocks(nullptr),
                _block_count(0)
            {
            }

            //! Disassembles the function and populates the list of blocks; a nop if the function has already been disassembled.
            void function::calculate_blocks(il_disassembler& disassembler)
            {
                // If we've already disassembled or cannot disassemble, do nothing
                // In case of Assembly.Load/AssemblyLoadContext.LoadFromStream, we want to instrument, even if there are no symbols
                if (_blocks || (!has_symbolic_information() && !_is_assembly_load))
                    return;

                instruction* last_inst = nullptr;
                instruction** instructions;
                size_t count = disassembler.get_instructions(/* [out] */ instructions);
                for (instruction** it = instructions; it < instructions + count; last_inst = *it, it++)
                {
                    //1.The tail prefix instruction must immediately precede a Call, Calli, or Callvirt instruction
                    //2.The instruction following the call instruction must be a ret.
                    //so we cannot put a probe after the call instrution as it will make IL invalid, we wait till we pass the ret instruction.
                    //Bug 167554:Tests pass in regular runs, but fail in coverage run
                    if ((*it)->get_opcode() == ILOrdinalOpcode::Cee_Tailcall)
                    {
                        it++;
                        while (!(*it)->is_return())
                        {
                            it++;
                        }
                    }
                    else
                    {
                        // 1. Mark instructions whose previous instructions are not fallthrough as start of a block
                        if (!last_inst || !last_inst->is_fall_through())
                        {
                            (*it)->set_starts_block(true);
                            // For Assembly.Load / AssemblyLoadContext.LoadFromStream, we need to instrument the function start
                            // i.e., the first block 
                            if (_is_assembly_load)
                            {
                                break;
                            }
                        }

                        // 2. Mark the targets of branches as start of a block
                        if ((*it)->is_direct_branch() || (*it)->is_indirect_branch())
                        {
                            for (instruction** target_it = (*it)->get_targets(); target_it < (*it)->get_targets() + (*it)->get_target_count(); ++target_it)
                            {
                                (*target_it)->set_starts_block(true);
                            }
                        }
                    }
                }

                // Process the instruction map into blocks
                process_blocks(disassembler);
            }

            void function::process_blocks(il_disassembler& disassembler)
            {
                // If there we no instructions, it's invalid
                instruction** instructions;
                size_t inst_count = disassembler.get_instructions(/* [out] */ instructions);
                if (inst_count == 0)
                {
                    throw new exception();
                }

                //// First, mark any instructions referenced from the exception causes as starting blocks
                //exception_clause* exception_clauses;
                //size_t clause_count = 0; // disassembler.get_exception_clauses(/* [out] */ exception_clauses);
                //for (auto clause = exception_clauses; clause < exception_clauses + clause_count; clause++)
                //{
                //    if (clause->get_try_instruction())
                //    {
                //        clause->get_try_instruction()->set_starts_block(true);
                //    }
                //    if (clause->get_handler_instruction())
                //    {
                //        clause->get_handler_instruction()->set_starts_block(true);
                //    }
                //    if (clause->get_filter_instruction())
                //    {
                //        clause->get_filter_instruction()->set_starts_block(true);
                //    }
                //}

                // Copy the instructions and count the number of blocks
                vector<block_type> blocks;
                block_type* current_block = nullptr;
                vector<instruction*> current_instructions;
                size_t block_size = 0;
                for (instruction** it = instructions; it < instructions + inst_count; ++it)
                {
                    instruction* instr = *it;

                    // If we're starting a new block
                    if (instr->get_starts_block())
                    {
                        if (current_block)
                        {
                            size_t blockInstructionCount = current_instructions.size();
                            // Set the block size for the block we're finishing
                            current_block->set_size(block_size);
                            current_block->set_instruction_count(blockInstructionCount);
                            for (size_t i = 0; i < blockInstructionCount; i++)
                                current_block->get_instructions()[i] = current_instructions[i];
                        }

                        // Insert a new code block.
                        blocks.push_back(block_type(instr->get_offset()));
                        current_block = &blocks.back();
                        block_size = 0;
                        current_instructions.clear();
                    }

                    // Push the instruction into the current block
                    if (current_block)
                    {
                        block_size += instr->get_size();
                        current_instructions.push_back(instr);
                    }
                }

                if (current_block)
                {
                    current_block->set_size(block_size);
                    current_block->set_instruction_count(current_instructions.size());
                    for (size_t i = 0; i < current_instructions.size(); i++)
                        current_block->get_instructions()[i] = current_instructions[i];
                }

                _blocks = new block_type[_block_count = blocks.size()];
                for (size_t i = 0; i < _block_count; i++)
                    _blocks[i] = std::move(blocks[i]);
            }
        }
    }
}
