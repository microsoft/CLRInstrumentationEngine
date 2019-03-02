#pragma once

//#include "stdafx.h"

using namespace ATL;

struct method_info
{
public:
    method_info() : _token(mdTokenNil), _start_block_index(0) {}
    method_info(mdToken token) : _token(token), _start_block_index(0) {}

    ~method_info() { _block_indexes.clear(); }

    mdToken get_method_token() { return _token; }

    void set_start_index(size_t start_block_index) { _start_block_index = start_block_index; }
    size_t get_start_index() { return _start_block_index; }

    void add_block_index(size_t block_index) { _block_indexes.push_back(block_index); }
    vector<size_t> get_block_indexes() { return _block_indexes; }

private:
    mdToken _token;
    vector<size_t> _block_indexes; /* indices where probes are to be added */
    size_t _start_block_index;
};

struct module_info
{
public:
    module_info(IModuleInfo* modInfo) : _current_module_info(modInfo) {}
    ~module_info() { _instrumented_functions_list.clear(); }

    void add_instrumented_functions(method_info& method_info) { _instrumented_functions_list.insert(make_pair(method_info.get_method_token(), method_info)); }

    IModuleInfo *get_module() { return _current_module_info; }

    bool contains_instrumented_method(mdToken token);
    bool get_method_info(mdToken token, method_info& info);

private:
    unordered_map<mdToken, method_info> _instrumented_functions_list;
    IModuleInfo *_current_module_info;
};


