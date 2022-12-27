$currentLocation = Get-Location

.\scripts\dos2unix.ps1 $currentLocation
# .\..\..\scripts\dos2unix.ps1 $currentLocation\..\..\scripts\

# $version = [IO.File]::ReadAllText("$currentLocation\..\..\VERSION.txt")
# $version = $version.Trim()
# $versionParts = $version.Split(".")
# $versionParts[2] = [int]$versionParts[2] + 1
# $version = [String]::Join(".",$versionParts)

# $profilerContent = Get-Content -Path "$currentLocation\..\..\ClrProfiler\CorProfiler.cpp"
# $profilerContent = $profilerContent -replace "_VERSION_", $version

# Write-Host $profilerContent

# $profilerContent | Set-Content -Path "$currentLocation\..\..\ClrProfiler\CorProfiler.cpp"

# $path = -join($currentLocation, "\", $version, "\core-profiler")

# if (Test-Path $path)
# {
	# Write-Host "$path folder already exists... Will delete it..."
    # Remove-Item -Force -Recurse $path
# }

# New-Item -ItemType Directory -Force -Path $path
# cd $path

az login --identity
docker login instanabuildcontainers.azurecr.io -u instanabuildcontainers -p 78tTvDns9UfxZpLqSL6ezi0+zII2GX3p
docker pull instanabuildcontainers.azurecr.io/corerewriter_build_centos:0.0.5
docker pull instanabuildcontainers.azurecr.io/corerewriter_build_alpine:0.0.2

$nixPath = (($currentLocation -replace "\\","/") -replace ":","").ToLower().Trim("/")
$nixCurrentLocation = "/mnt/$nixPath"
Write-Host "$nixCurrentLocation"

docker run -v $nixCurrentLocation/:/core-clr-tracer instanabuildcontainers.azurecr.io/corerewriter_build_centos:0.0.5
Write-Host "Copying Centos files"
copy .\..\..\..\..\ClrProfiler\CoreProfiler.so CoreProfiler.Centos.so
Write-Host "Files copied"

# docker run -v $nixCurrentLocation/../../:/core-clr-tracer instanabuildcontainers.azurecr.io/corerewriter_build_alpine:0.0.2
# Write-Host "Copying Alpine files"
# copy .\..\..\..\..\ClrProfiler\CoreProfiler.so CoreProfiler.Alpine.so
# Write-Host "Files copied"

Write-Host "Done"
#cd ..
#cd ..
