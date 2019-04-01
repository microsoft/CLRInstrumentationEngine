#include "stdafx.h"
#include "VanguardInstrumentation.h"

bool module_info::contains_instrumented_method(mdToken token)
{
    return _instrumented_functions_list.find(token) != _instrumented_functions_list.end();
}

bool module_info::get_method_info(mdToken token, method_info& info)
{
    unordered_map<mdToken, method_info>::iterator it = _instrumented_functions_list.find(token);
    if (it != _instrumented_functions_list.end())
    {
        info = it->second;
        return true;
    }

    return false;
}

void module_info::set_block_count(size_t block_count)
{
    _block_count = block_count;
    for (size_t i = 0; i < block_count; ++i)
    {
        _coverage_buffer.push_back(0);
    }

}