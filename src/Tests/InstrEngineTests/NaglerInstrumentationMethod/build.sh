[ -z "${CORECLR_PATH:-}" ] && CORECLR_PATH=/mnt/c/Users/ZZ00ZJ707/source/repos/CLRInstrumentationEngine/src/runtime-main/src/coreclr
[ -z "${BuildArch:-}"    ] && BuildArch=x64
[ -z "${BuildType:-}"    ] && BuildType=Debug
[ -z "${BuildOS:-}"    ] && BuildOS=Linux
[ -z "${Output:-}"       ] && Output=NaglerInstrumentationMethod.so
printf '  CORECLR_PATH : %s\n' "$CORECLR_PATH"
printf '  BuildOS      : %s\n' "$BuildOS"
printf '  BuildArch    : %s\n' "$BuildArch"
printf '  BuildType    : %s\n' "$BuildType"
printf '  Building %s ... ' "$Output"

CXX_FLAGS="$CXX_FLAGS -O2 -Wno-macro-redefined -Wno-invalid-noreturn -fPIC -fms-extensions -DBIT64 -DPAL_STDCPP_COMPAT -DPLATFORM_UNIX -std=c++11 -stdlib=libstdc++"
# INCLUDES="-I . -I $CORECLR_PATH/debug/daccess -I $CORECLR_PATH/pal/inc/rt -I $CORECLR_PATH/pal/prebuilt/inc -I $CORECLR_PATH/pal/inc -I $CORECLR_PATH/inc -I $CORECLR_PATH/bin/Product/$BuildOS.$BuildArch.$BuildType/inc"

# INCLUDES="-I . -I $CORECLR_PATH/debug/daccess -I $CORECLR_PATH/pal/inc/rt -I $CORECLR_PATH/pal/prebuilt/inc -I $CORECLR_PATH/pal/inc -I $CORECLR_PATH/inc -I /mnt/c/Users/ZZ00ZJ707/source/repos/CLRInstrumentationEngine/src/runtime-main/artifacts/bin/coreclr/Linux.x64.Debug/inc -I $CORECLR_PATH/../../../src/unix/inc/midl -I $CORECLR_PATH/../../../unix/inc/atl -I $CORECLR_PATH/jitjitstd"

INCLUDES="-I . -I $CORECLR_PATH/debug/daccess -I /mnt/c/Users/ZZ00ZJ707/source/repos/CLRInstrumentationEngine/out/Linux/CoreCLRPAL/inc/rt -I /mnt/c/Users/ZZ00ZJ707/source/repos/CLRInstrumentationEngine/out/Linux/CoreCLRPAL/inc -I /mnt/c/Users/ZZ00ZJ707/source/repos/CLRInstrumentationEngine/out/Linux/Intermediate -I $CORECLR_PATH/inc -I /mnt/c/Users/ZZ00ZJ707/source/repos/CLRInstrumentationEngine/src/runtime-main/artifacts/bin/coreclr/Linux.x64.Debug/inc -I $CORECLR_PATH/../../../unix/inc/midl -I $CORECLR_PATH/../../../unix/inc/atl -I $CORECLR_PATH/jitjitstd -I $CORECLR_PATH/../../../unix/inc/empty"

# INCLUDES="-I . -I $CORECLR_PATH/../../../src/unix/inc/midl -I $CORECLR_PATH/../../../unix/inc/atl -I $CORECLR_PATH/../../../../out/packages/microsoft.visualstudio.debugger.coreclrpal/3.0.0-20191022-01/inc/rt"

if [ $BuildOS=="MacOSX" ]; then
        CXX_FLAGS+=" -Wl,-undefined,error"
else
        CXX_FLAGS+=" --no-undefined"
fi

clang++ -shared -o $Output $CXX_FLAGS $INCLUDES NaglerInstrumentationMethod.cpp InstrumentationEngineApi.cpp stdafx.cpp dllmain.cpp

if [ $? -eq 0 ]
then
  echo "The rewriter has been compiled successfully. Copying to output."
  cp ./NaglerInstrumentationMethod.so ../NaglerInstrumentationMethod.so
  exit 0
else
  echo "The rewriter could not he built:" >&2
  exit 1
fi