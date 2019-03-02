#pragma once

#include "stdafx.h"
#include "managed_instruction.h"
#include "managed_function.h"
#include "VanguardInstrumentation.h"
#include <map>

using namespace vanguard::instrumentation::managed;

struct il_disassembler
{
public:
    il_disassembler(IModuleInfo *pModuleInfo);

    virtual void disassemble_function();
    virtual void initialize_function(IMethodInfo *methodInfo) { _current_method_info = methodInfo; }
    virtual size_t get_instructions(/* [out] */ instruction **&instructions);
    virtual void cleanup_function() {}
    virtual bool instrument_function(size_t block_index);

    virtual module_info *get_module_info() { return _module_info; }

private:
    template<typename source_type, typename target_type>
    target_type lookup(source_type source, std::map<source_type, target_type> &map, target_type default_value)
    {
        if (source && map.find(source) != map.end())
        {
            return map.find(source)->second;
        }

        return default_value;
    }

    DISCEE::TRMTA get_termination_type(IInstruction *il_inst)
    {
        ILOrdinalOpcode opcode;
        il_inst->GetOpCode(&opcode);
        if (opcode < 0)
        {
            return DISCEE::trmtaUnknown;
        }

        switch (opcode)
        {
        case Cee_Break:
            return DISCEE::trmtaTrap;
        case Cee_Switch:
            return DISCEE::trmtaBraSwitch;
        case Cee_Ret:
        case Cee_Endfinally:
        case Cee_Endfilter:
            return DISCEE::trmtaRet;
        case Cee_Call:
        case Cee_Jmp:
        case Cee_Newobj:
            return DISCEE::trmtaCall;
        case Cee_Calli:
        case Cee_Callvirt:
            return DISCEE::trmtaCallInd;
        case Cee_Br_S:
        case Cee_Br:
        case Cee_Leave:
        case Cee_Leave_S:
            return DISCEE::trmtaBra;
        case Cee_Throw:
        case Cee_Rethrow:
            return DISCEE::trmtaBraInd;
        }

        if (opcode >= Cee_Brfalse_S && opcode < Cee_Switch)
        {
            return DISCEE::trmtaBraCc;
        }

        // For the rest (NEXT and META), they are fallthrough
        return DISCEE::trmtaFallThrough;
    }

    bool _use_verifiable_probe;
    IMethodInfo *_current_method_info;
    std::vector<vanguard::instrumentation::managed::instruction*> _instructions;
    std::vector<IInstruction*> _il_instructions;
    module_info *_module_info;
    size_t _global_block_count;
};