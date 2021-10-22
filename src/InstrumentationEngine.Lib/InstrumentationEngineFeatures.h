#pragma once

namespace MicrosoftInstrumentationEngine
{
    class InstrumentationEngineFeatures final
    {
        private:
            static constexpr const WCHAR* s_userBuffer = _T("MicrosoftInstrumentationEngine_UserBuffer");
        public:
            static bool UserBufferEnabled()
            {
                return GetEnvironmentVariable(s_userBuffer, nullptr, 0) > 0;
            }
    };
}