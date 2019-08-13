// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

struct ICodeInjector;

typedef std::shared_ptr<ICodeInjector> ICodeInjectorSPtr;
typedef std::weak_ptr<ICodeInjector> ICodeInjectorWPtr;