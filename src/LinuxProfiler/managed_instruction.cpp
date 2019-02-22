#include "stdafx.h"
#include "managed_instruction.h"

namespace vanguard {
    namespace instrumentation {
        namespace managed
        {
            //! Gets whether or not the instruction's termination type is a direct branch.
            bool instruction::is_direct_branch() const
            {
                return _termination == DISCEE::trmtaBra || is_conditional_branch(); // In IL, all conditional branches are direct
            }

            //! Gets whether or not the instruction's termination type is a conditional branch.
            bool instruction::is_conditional_branch() const
            {
                return _termination == DISCEE::trmtaBraCc || is_switch();     // Switches are also conditional
            }

            //! Gets whether or not the instruction's termination type is an indirect branch.
            bool instruction::is_indirect_branch() const
            {
                return _termination == DISCEE::trmtaBraInd;
            }

            //! Gets whether or not the instruction's termination type is a indirect call.
            bool instruction::is_indirect_call() const
            {
                return _termination == DISCEE::trmtaCallInd;
            }

            //! Gets whether or not the instruction's termination type is a direct call.
            bool instruction::is_direct_call() const
            {
                return _termination == DISCEE::trmtaCall;
            }

            //! Gets whether or not the instruction's termination type is a return.
            bool instruction::is_return() const
            {
                return _termination == DISCEE::trmtaRet;
            }

            //! Gets whether or not the instruction's termination type is falling through to the next instruction.
            bool instruction::is_fall_through() const
            {
                return _termination == DISCEE::trmtaFallThrough;
            }

            //! Gets whether or not the instruction's termination type is a trap instruction (break).
            bool instruction::is_trap() const
            {
                return _termination == DISCEE::trmtaTrap;
            }

            //! Gets whether or not the instruction's termination type is a switch.
            bool instruction::is_switch() const
            {
                return _termination == DISCEE::trmtaBraSwitch;
            }
        }
    }
}