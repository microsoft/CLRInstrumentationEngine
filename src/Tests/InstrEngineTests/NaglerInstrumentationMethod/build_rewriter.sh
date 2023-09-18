[ -z "${CORECLR_PATH:-}" ] && CORECLR_PATH=/coreclr
[ -z "${BuildArch:-}"    ] && BuildArch=x64
[ -z "${BuildType:-}"    ] && BuildType=Release
[ -z "${Output:-}"       ] && Output=NaglerInstrumentationMethod.so
printf '  CORECLR_PATH : %s\n' "$CORECLR_PATH"
printf '  BuildOS      : %s\n' "$BuildOS"
printf '  BuildArch    : %s\n' "$BuildArch"
printf '  BuildType    : %s\n' "$BuildType"
printf '  Building %s ... ' "$Output"

CXX_FLAGS="$CXX_FLAGS -O2 -Wno-macro-redefined -Wno-invalid-noreturn -fPIC -fms-extensions -DBIT64 -DPAL_STDCPP_COMPAT -DPLATFORM_UNIX -std=c++11 -stdlib=libstdc++"
INCLUDES="-i . -i $coreclr_path/debug/daccess -i $coreclr_path/pal/inc/rt -i $coreclr_path/pal/prebuilt/inc -i $coreclr_path/pal/inc -i $coreclr_path/inc -i $coreclr_path/jit/jitstd -i $coreclr_path/bin/product/$buildos.$buildarch.$buildtype/inc -i $coreclr_path/../../../unix/inc/atl"

# INCLUDES="-I . -I /mnt/c/"Program Files"/"Microsoft Visual Studio"/2022/Professional/VC/Tools/MSVC/14.31.31103/include -I /mnt/c/"Program Files"/"Microsoft Visual Studio"/2022/Professional/VC/Tools/MSVC/14.31.31103/atlmfc/include -I /mnt/c/"Program Files"/"Microsoft Visual Studio"/2022/Professional/VC/Tools/MSVC/14.31.31103/include/msclr -I /mnt/c/"Program Files (x86)"/"Windows Kits"/10/Include/10.0.19041.0/um -I /mnt/c/"Program Files (x86)"/"Windows Kits"/10/Include/10.0.19041.0/shared -I /mnt/c/"Program Files (x86)"/"Windows Kits"/10/Include/10.0.19041.0/ucrt  -I /mnt/c/"Program Files (x86)"/Windows\ Kits/10/Include/10.0.19041.0/ucrt/sys -I /mnt/c/Users/ZZ00ZJ707/source/repos/CLRInstrumentationEngine/inc/clr/prof  -I /mnt/c/"Program Files (x86)"/"Windows Kits"/NETFXSDK/4.8/Include/um  -I /mnt/c/Users/ZZ00ZJ707/source/repos/CLRInstrumentationEngine/inc/clr/extra -I /mnt/c/Users/ZZ00ZJ707/source/repos/CLRInstrumentationEngine/src/Common.Lib -I /mnt/c/Users/ZZ00ZJ707/source/repos/CLRInstrumentationEngine/src/Common.Headers  -I /mnt/c/Users/ZZ00ZJ707/source/repos/CLRInstrumentationEngine/src/InstrumentationEngine.Api"

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