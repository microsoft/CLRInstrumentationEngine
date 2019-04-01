#pragma once

#include "stdafx.h"
#include "block.h"
#include "managed_instruction.h"
#include <functional>

namespace vanguard {
    namespace instrumentation {
        namespace managed
        {
            struct il_disassembler;

            //! Represents a managed function.
            struct function
            {
                static const DWORD INVALID_ID = 0xFFFFFFFF;
                typedef block<instruction*, size_t> block_type;

                function();

                //! Move constructor for function.
                function(function&& other)
                {
                    *this = std::move(other);
                }

                //! Destructor
                ~function()
                {
                    if (_blocks) delete[] _blocks;
                }

                //! Move assignment operator for function.
                function& operator=(function&& other)
                {
                    _id = other._id;
                    _rva = other._rva;
                    _token = other._token;
                    _type_token = other._type_token;
                    _address = other._address;
                    _blocks = other._blocks;
                    _block_count = other._block_count;
                    other._blocks = nullptr;
                    _size = other._size;
                    _is_assembly_load = other._is_assembly_load;
                    return *this;
                }

                //! Analyze the blocks inside the function.
                void calculate_blocks(il_disassembler& disassembler);

                //! Determines if there is enough symbolic information to instrument this function.
                bool has_symbolic_information() const
                {
                    return
                        _id != -1 &&
                        _rva != 0 &&
                        _token != 0 &&
                        _address &&
                        wcslen(_name) > 0 &&
                        wcslen(_source_file) > 0;
                }

                //! Gets the function's identifier.
                uint32_t get_id() const { return _id; }

                //! Gets the function's RVA.
                uint32_t get_rva() const { return _rva; }

                //! Gets the function's metadata token.
                uint32_t get_token() const { return _token; }

                //! Sets the function's metadata token.
                void set_token(uint32_t token) { _token = token; }

                //! Gets the function's type's metadata token.
                uint32_t get_type_token() const { return _type_token; }

                //! Get tokens of property this method is associated with. Usually there are zero or one such properties
                const std::vector<uint32_t>& get_property_tokens() const { return _property_tokens; }

                //! Gets the function's address.
                _Ret_notnull_ void const* get_address() const { return _address; }

                //! Gets the function's size in bytes.
                size_t get_size() const { return _size; }

                //! Sets the function's size in bytes.
                void set_size(size_t size) { _size = size; }

                //! Gets the function's name.
                const wchar_t* get_name() const { return _name; }

                //! Gets the function's type name.
                const wchar_t* get_type_name() const { return _type_name; }

                //! Gets the function's namespace name.
                const wchar_t* get_namespace_name() const { return _namespace_name; }

                //! Gets the function's source file.
                const wchar_t* get_source_file() const { return _source_file; }

                //! Gets the blocks of the function.
                //! Disassemble must be called before this list will be populated.
                block_type* const get_blocks() const { return _blocks; }

                //! Gets the block count.
                size_t get_block_count() const { return _block_count; }

                static const std::wstring separator;

            protected:
                //! Constructs a function given the symbol and metadata.

                const wchar_t* _name;
                const wchar_t* _type_name;
                const wchar_t* _namespace_name;
                const wchar_t* _source_file;

            private:
                //! Function does not implement correct copy constructor/operator, make them private.
                function(const function&);
                const function& operator=(const function&);

                void process_blocks(il_disassembler& disassembler);

                uint32_t _id;
                uint32_t _rva;
                uint32_t _token;
                uint32_t _type_token;
                std::vector<mdToken> _property_tokens;
                void const* _address;
                block_type* _blocks;
                size_t _block_count;
                size_t _size;

                bool _is_assembly_load;
            };
        }
    }
}