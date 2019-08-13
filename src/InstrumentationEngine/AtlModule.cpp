// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"

#include "AtlModule.h"

MicrosoftInstrumentationEngine::CCustomAtlModule _AtlModule;

MicrosoftInstrumentationEngine::CCustomAtlModule::CCustomAtlModule()
{
    (void)LoadResourceModule();
}

MicrosoftInstrumentationEngine::CCustomAtlModule::~CCustomAtlModule()
{
    FreeResourceModule();
}

HRESULT MicrosoftInstrumentationEngine::CCustomAtlModule::LoadResourceModule()
{
    // TODO: Need to figure out localization story.
    return S_OK;
}

void MicrosoftInstrumentationEngine::CCustomAtlModule::FreeResourceModule()
{
    // TODO: Need to figure out localization story.
}