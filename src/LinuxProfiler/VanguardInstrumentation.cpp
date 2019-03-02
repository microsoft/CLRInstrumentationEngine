#include "stdafx.h"
#include "VanguardInstrumentation.h"

using namespace vanguard::instrumentation::managed;

//bool module_info::contains_instrumented_method(mdToken token)
//{
//    return _instrumented_functions_list.find(token) != _instrumented_functions_list.end();
//}

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