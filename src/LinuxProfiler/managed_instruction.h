#pragma once
#include "stdafx.h"

namespace DISCEE
{
    enum TRMTA
    {
        trmtaTrap,
        trmtaBraSwitch,
        trmtaRet,
        trmtaCall,
        trmtaCallInd,
        trmtaBra,
        trmtaBraInd,
        trmtaBraCc,
        trmtaFallThrough,
        trmtaUnknown
    };
};

namespace vanguard {
    namespace instrumentation {
        namespace managed {

            //! Represents environment independent instruction
            struct instruction
            {
                //! Constructor
                instruction(size_t offset, size_t instruction_size, DISCEE::TRMTA termination, size_t index, ILOrdinalOpcode opcode) :
                    _offset(offset),
                    _instruction_size(instruction_size),
                    _starts_block(false),
                    _termination(termination),
                    _index(index),
                    _targets(nullptr),
                    _target_count(0),
                    _opcode(opcode)
                {
                }

                //! Move constructor for instruction.
                instruction(instruction&& other)
                {
                    *this = std::move(other);
                }

                //! Destructor
                ~instruction()
                {
                    if (_targets) delete[] _targets;
                }

                //! Move assignment operator for instruction.
                instruction& operator=(instruction&& other)
                {
                    _termination = other._termination;
                    _starts_block = other._starts_block;
                    _index = other._index;
                    _offset = other._offset;
                    _instruction_size = other._instruction_size;
                    _targets = other._targets;
                    _target_count = other._target_count;
                    _opcode = other._opcode;
                    other._targets = nullptr;

                    return *this;
                }

                //! Gets whether or not this instruction is the start of a basic block.
                bool get_starts_block() const { return _starts_block; }

                //! Sets whether or not this instruction is the start of a basic block.
                void set_starts_block(bool value) { _starts_block = value; }

                //! Gets whether or not the instruction's termination type is a direct branch.
                bool is_direct_branch() const;

                //! Gets whether or not the instruction's termination type is a conditional branch.
                bool is_conditional_branch() const;

                //! Gets whether or not the instruction's termination type is an indirect branch.
                bool is_indirect_branch() const;

                //! Gets whether or not the instruction's termination type is a indirect call.
                bool is_indirect_call() const;

                //! Gets whether or not the instruction's termination type is a direct call.
                bool is_direct_call() const;

                //! Gets whether or not the instruction's termination type is a return.
                bool is_return() const;

                //! Gets whether or not the instruction's termination type is falling through to the next instruction.
                bool is_fall_through() const;

                //! Gets whether or not the instruction's termination type is a trap instruction (break).
                bool is_trap() const;

                //! Gets whether or not the instruction's termination type is a switch.
                bool is_switch() const;

                //! Gets the index of the instruction (unique in its containing function)
                size_t get_index() const { return _index; }

                //! Gets the targets of the instruction.
                instruction** get_targets() { return _targets; }

                //! Gets the targets of the instruction.
                instruction** const get_targets() const { return _targets; }

                //! Gets the target count.
                size_t get_target_count() const { return _target_count; }

                //! Sets the target count.
                void set_target_count(size_t count)
                {
                    if (_targets) delete[] _targets;
                    _targets = new instruction*[_target_count = count];
                }

                //! Gets the instruction's offset relative to the start of the function.
                size_t get_offset() const { return _offset; }

                //! Gets the size of the instruction in bytes.
                size_t get_size() const { return _instruction_size; }

                ILOrdinalOpcode get_opcode() const { return _opcode; }

            private:
                //! Instruction does not implement correct copy constructor/operator, make them private.
                instruction(const instruction&);
                const instruction& operator=(const instruction&);

                bool _starts_block;
                DISCEE::TRMTA _termination;
                ILOrdinalOpcode _opcode;
                size_t _index;
                size_t _offset;
                instruction** _targets;
                size_t _target_count;
                size_t _instruction_size;

            };
        }
    }
}