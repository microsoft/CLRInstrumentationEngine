// Copyright (c) Microsoft Corporation. All rights reserved.
//

#pragma once

struct ICodeInjector;

typedef std::shared_ptr<ICodeInjector> ICodeInjectorSPtr;
typedef std::weak_ptr<ICodeInjector> ICodeInjectorWPtr;