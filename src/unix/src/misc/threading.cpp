// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include <iostream>
#include <pthread.h>
#include <mutex>
#include <queue>

using namespace std;

typedef struct _THREADPOOL_DATA
{
    LPTHREAD_START_ROUTINE func;
    PVOID context;
    DWORD retval;
}THREADPOOL_DATA;

const int MAX_THREADPOOL_THREADS = 128;
std::queue<THREADPOOL_DATA> pending_items;
int current_running = 0;
// Lock used to sync access to pending_items and current_running.
std::mutex g_lock;

static void * thread_start_func(void *arg)
{
    THREADPOOL_DATA data;

    while (true)
    {
        // Dequeue
        {
            lock_guard<mutex> lock(g_lock);
            if (pending_items.size() == 0)
            {
                current_running --;
                break;
            }
            data = pending_items.front();
            pending_items.pop();
        }

        // Run the thread function.
        data.retval = data.func(data.context);
    }

    return NULL;
}

// Primitive implementation of QueueUserWorkItem
PALAPI BOOL QueueUserWorkItem(LPTHREAD_START_ROUTINE Function, PVOID Context, ULONG Flags)
{
    const ULONG allowedFlags = (WT_EXECUTEDEFAULT | WT_EXECUTELONGFUNCTION);
    BOOL retval = TRUE;

    PERF_ENTRY(QueueUserWorkItem);
    ENTRY("QueueUserWorkItem");


    THREADPOOL_DATA data;
    data.func  = Function;
    data.context = Context;

    if (0 != (Flags & ~allowedFlags))
    {
        ERROR("Only the following flags are allowed: %u\n", allowedFlags);
        SetLastError(ERROR_INVALID_PARAMETER);
        retval = FALSE;
    }
    else
    {
        bool create_thread = false;

        {
            lock_guard<mutex> lock(g_lock);
            pending_items.push(data);
            if (current_running < MAX_THREADPOOL_THREADS)
            {
                current_running ++;
                create_thread = true;
            }
        }
        if (create_thread)
        {
            pthread_t t1;
            int unix_err = pthread_create(&t1, NULL, thread_start_func, NULL);
            if (0 != unix_err)
            {
                retval = FALSE;
                SetLastError(ERROR_BUSY);
                ERROR("Failed to create pthread. Error code : %d\n", unix_err);
                lock_guard<mutex> lock(g_lock);
                pending_items.pop();
                current_running --;
            }
        }
    }
    LOGEXIT("QueueUserWorkItem returning %d\n", retval);
    PERF_EXIT(QueueUserWorkItem);
    return retval;
}
