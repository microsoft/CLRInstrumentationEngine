#include "stdafx.h"
#include "VanguardInstrumentation.h"
#include "ILDisassembler.h"

using namespace vanguard::instrumentation::managed;

bool module_info::contains_instrumented_method(mdToken token)
{
    return _instrumented_functions_list.find(token) != _instrumented_functions_list.end();
}

bool module_info::get_method_info(mdToken token, method_info& info)
{
    map<mdToken, method_info>::iterator it = _instrumented_functions_list.find(token);
    if (it != _instrumented_functions_list.end())
    {
        info = it->second;
        return true;
    }

    return false;
}


il_disassembler::il_disassembler(IModuleInfo *pModuleInfo)
{
    _module_info = new module_info(pModuleInfo);
}

void il_disassembler::disassemble_function()
{
    int index = 0;
    map<IInstruction*, instruction*> inst_dict;

    IInstructionGraph* instructionsGraph;
    HRESULT hr = _current_method_info->GetInstructions(&instructionsGraph);
    std::vector<IInstruction*> _il_instructions;

    if (hr == S_OK)
    {
        IInstruction* il_inst = NULL;

        HRESULT instructionData = instructionsGraph->GetFirstInstruction(&il_inst);

        DWORD pOffset;
        DWORD pSize;
        ILOrdinalOpcode pOpCode;

        il_inst->GetOffset(&pOffset);
        il_inst->GetInstructionLength(&pSize);
        il_inst->GetOpCode(&pOpCode);

        instruction *inst = new instruction((size_t)pOffset, (size_t)pSize, get_termination_type(il_inst), index++, pOpCode);
        _il_instructions.push_back(il_inst);
        _instructions.push_back(inst);
        inst_dict.insert(std::pair<IInstruction*, instruction*>(il_inst, inst));

        while (instructionData == S_OK)
        {
            IInstruction* nextInstruction = NULL;
            instructionData = il_inst->GetNextInstruction(&nextInstruction);

            if (instructionData != S_OK)
            {
                break;
            }

            _il_instructions.push_back(nextInstruction);

            nextInstruction->GetOffset(&pOffset);
            nextInstruction->GetInstructionLength(&pSize);
            nextInstruction->GetOpCode(&pOpCode);

            inst = new instruction((size_t)pOffset, (size_t)pSize, get_termination_type(il_inst), index++, pOpCode);
            _instructions.push_back(inst);

            il_inst = nextInstruction;
            inst_dict.insert(std::pair<IInstruction*, instruction*>(il_inst, inst));
        }
    }

    // Set targets
    for (size_t i = 0; i < _il_instructions.size(); i++)
    {
        BOOL isSwitch, isBranch;
        _il_instructions[i]->GetIsSwitch(&isSwitch);
        _il_instructions[i]->GetIsBranch(&isBranch);

        if (isSwitch)
        {
            ISwitchInstruction *il_switch;
            _il_instructions[i]->QueryInterface(__uuidof(ISwitchInstruction), (LPVOID*)&il_switch);

            DWORD pBranchCount = 0;
            il_switch->GetBranchCount(&pBranchCount);

            _instructions[i]->set_target_count(pBranchCount);

            for (ULONG j = 0; j < pBranchCount; j++)
            {
                IInstruction *pBranchTarget;

                il_switch->GetBranchTarget(j, &pBranchTarget);
                _instructions[i]->get_targets()[j] = lookup(pBranchTarget, inst_dict, (instruction*)nullptr);
            }
        }
        else if (isBranch)
        {
            IBranchInstruction *il_branch;
            _il_instructions[i]->QueryInterface(__uuidof(IBranchInstruction), (LPVOID*)&il_branch);
            instruction *target = nullptr;
            IInstruction* pBranchTarget;
            il_branch->GetBranchTarget(&pBranchTarget);

            if (target == lookup(pBranchTarget, inst_dict, target))
            {
                _instructions[i]->set_target_count(1);
                _instructions[i]->get_targets()[0] = target;
            }
        }
    }

    /*for (auto clause = _current_exception_section->Clauses().GetFirst(); clause; clause = _current_exception_section->Clauses().GetNext(clause))
    {
        _exception_clauses.push_back(exception_clause(
            lookup(clause->_tryFirstInstruction, inst_dict, (instruction*)nullptr),
            lookup(clause->_handlerFirstInstruction, inst_dict, (instruction*)nullptr),
            lookup(clause->_filterFirstInstruction, inst_dict, (instruction*)nullptr)));
    }*/
}

size_t il_disassembler::get_instructions(vanguard::instrumentation::managed::instruction **&instructions)
{
    instructions = _instructions.data();
    return _instructions.size();
}

bool il_disassembler::instrument_function(size_t block_index)
{
    mdToken methodToken;
    _current_method_info->GetMethodToken(&methodToken);
    method_info info(methodToken);

    /*for (auto block_it = _current_function->get_blocks(); block_it < _current_function->get_blocks() + _current_function->get_block_count(); ++block_it)
    {
        auto inst = block_it->get_instructions()[0];
        info.add_block_index(inst->get_index());
    }

    info.set_start_index(_global_block_count);
    _module_info->add_instrumented_functions(info);

    _global_block_count += _current_function->get_block_count();
*/
    return true;
}

void il_disassembler::cleanup_function()
{
    for (vector<instruction*>::iterator it = _instructions.begin(); it != _instructions.end(); ++it)
    {
        delete *it;
    }

    _instructions.clear();
    _il_instructions.clear();
    _current_method_info = nullptr;
    //_current_exception_section = nullptr;
}
