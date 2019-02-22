#pragma once

#include "stdafx.h"

namespace vanguard {
    namespace instrumentation
    {
        //! Represents a basic block.
        template <typename instr_type, typename address_type>
        struct block
        {
            typedef instr_type instruction_type;

            //! Constructs a basic block given its starting address.
            block(address_type address = 0) :
                _address(address),
                _size(0),
                _is_prolog_block(false),
                _instructions(nullptr),
                _instruction_count(0),
                _is_catch_block(false)
            {
            }

            //! Move constructor for basic block.
            block(block&& other)
            {
                *this = std::move(other);
            }

            //! Destructor
            ~block()
            {
                if (_instructions) delete[] _instructions;
            }

            //! Move assignment operator for basic block.
            block& operator=(block&& other)
            {
                _address = other._address;
                _size = other._size;
                _instructions = other._instructions;
                _instruction_count = other._instruction_count;
                other._instructions = nullptr;
                _is_prolog_block = other._is_prolog_block;
                _is_catch_block = other._is_catch_block;
                return *this;
            }

            //! Gets the starting address of the basic block.
            address_type get_address() const { return _address; }

            //! Gets the size of the basic block.
            size_t get_size() const { return _size; }

            //! Sets the size of the basic block.
            void set_size(size_t size) { _size = size; }

            //! Gets whether or not the block is the prolog block (only applies to x64).
            bool is_prolog() const { return _is_prolog_block; }

            // Return true if block is catch handler
            bool is_catch_block() const { return _is_catch_block; }

            //! Sets whether or not the block is the prolog block (only applies to x64).
            void set_prolog(bool value) { _is_prolog_block = value; }

            // Sets whether or not block is of catch handler type
            void set_catch_block(bool value) { _is_catch_block = value; }

            //! Gets the instructions that are part of the basic block.
            instruction_type* get_instructions() { return _instructions; }

            //! Gets the instructions that are part of the basic block.
            instruction_type* const get_instructions() const { return _instructions; }

            //! Gets instruction count.
            size_t get_instruction_count() const { return _instruction_count; }

            //! Sets instruction count.
            void set_instruction_count(size_t count)
            {
                if (_instructions) delete[] _instructions;
                _instructions = new instruction_type[_instruction_count = count];
            }

        private:
            //! Block does not implement correct copy constructor/operator, make them private.
            block(const block&);
            const block& operator=(const block&);

            address_type _address;
            size_t _size;
            instruction_type* _instructions;
            size_t _instruction_count;
            bool _is_prolog_block;
            bool _is_catch_block;
        };
    }
}