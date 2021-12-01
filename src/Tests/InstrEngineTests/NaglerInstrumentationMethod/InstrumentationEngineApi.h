#pragma once
namespace InstrumentationEngineApi
{
    /// <summary>
    /// Frees a string allocated by the Instrumentation Engine.
    /// </summary>
    /// <param name="bstr">The string to free</param>
    /// <returns>S_OK for success. Failure otherwise.</returns>
    HRESULT FreeString(BSTR bstr);

};

