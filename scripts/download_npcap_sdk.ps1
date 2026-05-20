# download_npcap_sdk.ps1
# Downloads and extracts the npcap SDK for Windows builds.
# This script is used for release builds where we need the real npcap SDK
# (mock libraries are used for CI builds).
#
# Usage:
#   .\scripts\download_npcap_sdk.ps1 [-OutputDir <path>]
#
# The npcap SDK can be downloaded from:
#   https://npcap.com/dist/npcap-sdk-1.13.zip
#   (or newer version as needed)

param(
    [string]$OutputDir = "$PSScriptRoot\..\npcap-sdk",
    [string]$NpcapVersion = "1.13",
    [string]$NpcapUrl = "https://npcap.com/dist/npcap-sdk-${NpcapVersion}.zip"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

Write-Host "Downloading npcap SDK v${NpcapVersion} from ${NpcapUrl}..."

# Create output directory
if (-not (Test-Path $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
}

# Download the SDK
$TempZip = Join-Path $env:TEMP "npcap-sdk-${NpcapVersion}.zip"
try {
    Invoke-WebRequest -Uri $NpcapUrl -OutFile $TempZip -UseBasicParsing
} catch {
    Write-Error "Failed to download npcap SDK from ${NpcapUrl}"
    Write-Error $_.Exception.Message
    exit 1
}

# Extract the SDK
Write-Host "Extracting npcap SDK to ${OutputDir}..."
try {
    Add-Type -AssemblyName System.IO.Compression.FileSystem
    [System.IO.Compression.ZipFile]::ExtractToDirectory($TempZip, $OutputDir)
} catch {
    Write-Error "Failed to extract npcap SDK"
    Write-Error $_.Exception.Message
    exit 1
}

# Clean up
Remove-Item $TempZip -Force -ErrorAction SilentlyContinue

Write-Host "npcap SDK v${NpcapVersion} downloaded and extracted to ${OutputDir}"
Write-Host "SDK files are now available at:"
Write-Host "  Headers: $(Join-Path $OutputDir 'Include')"
Write-Host "  Libraries: $(Join-Path $OutputDir 'Lib\x64')"

# Print the SDK path for CMake to use
Write-Host "NPCAP_SDK_PATH=${OutputDir}"
