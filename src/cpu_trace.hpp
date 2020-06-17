/*
 * MIT License
 *
 * Copyright (c) 2020 Martin Lafreniere
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef CHIP8_CPUTRACE_HPP
#define CHIP8_CPUTRACE_HPP

#include <cstdio>
#include <vector>

#include "cpu.hpp"

namespace chip8 {

/// @brief Represent a Cpu opcode tracing class.
class CpuTrace
{
    public:
        /// @brief Cpu information indices
        enum CpuInfoIndex : uint32_t
        {
            PC_INDEX    = 0,
            VX_INDEX    = 1,
            SP_INDEX    = 2,
            I_INDEX     = 3,
            DT_INDEX    = 4,
            ST_INDEX    = 5,
            STACK_INDEX = 6,

            INFO_COUNT
        };

        /// @brief Cpu information to trace.
        enum CpuInfo : uint32_t
        {
            NONE  = 0,
            PC    = (1 <<  PC_INDEX),
            VX    = (1 <<  VX_INDEX),
            SP    = (1 <<  SP_INDEX),
            I     = (1 <<  I_INDEX),
            DT    = (1 <<  DT_INDEX),
            ST    = (1 <<  ST_INDEX),
            STACK = (1 <<  STACK_INDEX)
        };


        /// @brief Construct a CPU trace with decoded operand(s).
        ///
        /// RIAA style to trace opcode and trace info before executing the
        /// opcode.
        ///
        /// @tparam OP Operand type.
        /// @param enabled    Trace is enabled.
        /// @param cpu        Reference to the CPU object.
        /// @param opcode     Current opcode to trace.
        /// @param op         Operand(s) decoded from opcode.
        /// @param infoFlags  Information to trace flags.
        template<typename OP>
        CpuTrace(bool enabled, Cpu * cpu, opcode::Opcode opcode, OP const& op, uint32_t infoFlags)
            : enabled_{ enabled }
            , cpu_{ cpu }
            , infoFlags_{ infoFlags }
        {
            if (enabled_ && infoFlags_ != NONE)
            {
                std::printf("Address: 0x%04X | ", cpu_->getRegContext().pc - Cpu::PC_INCR);
                opcode::trace(opcode, op);
                traceInfo();
            }
        }

        /// @brief Construct a CPU trace with decoded operand(s).
        ///
        /// RIAA style to trace opcode and info before executing the opcode.
        ///
        /// @tparam OP Operand type.
        /// @param enabled    Trace is enabled.
        /// @param cpu        Reference to the CPU object.
        /// @param opcode     Current opcode to trace.
        /// @param infoFlags  Information to trace flags.
        CpuTrace(bool enabled, Cpu * cpu, opcode::Opcode opcode, uint32_t infoFlags)
            : enabled_{ enabled }
            , cpu_{ cpu }
            , infoFlags_{ infoFlags }
        {
            if (enabled_ && infoFlags_ != NONE)
            {
                opcode::trace(opcode);
                traceInfo();
            }
        }

        /// @brief Destroy the CPU trace instance.
        ///
        /// Trace the info after executing the opcode.
        ~CpuTrace()
        {
            if (enabled_ && infoFlags_ != NONE)
            {
                traceInfo();
            }
        }

    private:
        using TraceFunc = void (CpuTrace::*)();
        using TraceTable = std::vector<TraceFunc>;

        static const TraceTable TRACE_TABLE;

        void traceProgramCounter()
        {
            std::printf("PC: 0x%04X;  ", cpu_->getRegContext().pc);
        }

        void traceGeneralPurposeRegisters()
        {
            for (uint32_t index = 0; index < Cpu::REG_COUNT; ++index)
            {
                std::printf("V%01X: 0x%02X;  ", index, cpu_->getRegContext().vx[index]);
            }
        }

        void traceStackPointer()
        {
            std::printf("SP: 0x%02X;  ", cpu_->getRegContext().sp);
        }

        void traceIRegister()
        {
            std::printf("I:  0x%04X;  ", cpu_->getRegContext().i);
        }

        void traceDelayRegister()
        {
            std::printf("DT: 0x%02X;  ", cpu_->getRegContext().dt);
        }

        void traceSoundRegister()
        {
            std::printf("ST: 0x%02X;  ", cpu_->getRegContext().dt);
        }

        void traceStack()
        {
            std::printf("Stack:");
            for (std::size_t index = 0; index <= cpu_->getRegContext().sp; ++index)
            {
                std::printf(" 0x%04X", cpu_->getRegContext().stack[index]);
            }
            std::puts("");
        }

        void traceInfo()
        {
            for (uint32_t index = PC_INDEX; index < STACK_INDEX; ++index)
            {
                uint32_t flag = (1 << index);

                if (infoFlags_ & flag)
                {
                    (this->*(TRACE_TABLE[index]))();
                }
            }

            if (infoFlags_ & ~STACK)
            {
                std::puts("");
            }

            if (infoFlags_ & STACK)
            {
                traceStack();
            }
        }

        bool     enabled_;
        Cpu *    cpu_;
        uint32_t infoFlags_;
};

/// @brief Trace table to call function based on flags.
const CpuTrace::TraceTable CpuTrace::TRACE_TABLE = {
    &CpuTrace::traceProgramCounter,
    &CpuTrace::traceGeneralPurposeRegisters,
    &CpuTrace::traceStackPointer,
    &CpuTrace::traceIRegister,
    &CpuTrace::traceDelayRegister,
    &CpuTrace::traceSoundRegister
};

} // namespace chip8

#endif // CHIP8_CPUTRACE_HPP
