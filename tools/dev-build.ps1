[CmdletBinding()]
param(
    [ValidateSet("Debug", "Release", "RelWithDebInfo", "MinSizeRel")]
    [string]$Config = "Debug",

    [ValidateSet("x64", "Win32", "ARM64")]
    [string]$Platform = "x64",

    [string]$BuildDir = "",

    [switch]$RunTests,
    [switch]$RunApp,
    [switch]$ForceNMake,
    [switch]$ForceNinja,
    [switch]$ForceMinGW
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Test-ToolAvailable {
    param([Parameter(Mandatory = $true)][string]$Name)
    return $null -ne (Get-Command $Name -ErrorAction SilentlyContinue)
}

function Get-AvailableGenerators {
    $helpText = & cmake --help
    if ($LASTEXITCODE -ne 0) {
        throw "Failed to query CMake generators (cmake --help)."
    }

    $generators = New-Object System.Collections.Generic.List[string]
    $inSection = $false
    foreach ($line in $helpText) {
        if ($line -match "^Generators$") {
            $inSection = $true
            continue
        }
        if (-not $inSection) {
            continue
        }
        if ($line -match "^\s*\*?\s*(.+?)\s*=\s+") {
            $generators.Add($matches[1].Trim()) | Out-Null
        }
    }
    return @($generators)
}

function New-Candidate {
    param(
        [string]$Name,
        [string]$Id,
        [bool]$NeedsPlatform,
        [bool]$MultiConfig,
        [string[]]$RequiredTools
    )
    return [pscustomobject]@{
        Name = $Name
        Id = $Id
        NeedsPlatform = $NeedsPlatform
        MultiConfig = $MultiConfig
        RequiredTools = $RequiredTools
    }
}

function Get-CandidateBuildDir {
    param(
        [Parameter(Mandatory = $true)][pscustomobject]$Candidate,
        [Parameter(Mandatory = $true)][string]$ConfigName,
        [string]$UserBuildDir
    )
    if (-not [string]::IsNullOrWhiteSpace($UserBuildDir)) {
        return $UserBuildDir
    }

    $configLower = $ConfigName.ToLowerInvariant()
    return "build/$($Candidate.Id)-$configLower"
}

function Invoke-Configure {
    param(
        [Parameter(Mandatory = $true)][pscustomobject]$Candidate,
        [Parameter(Mandatory = $true)][string]$BuildPath,
        [Parameter(Mandatory = $true)][string]$ConfigName,
        [Parameter(Mandatory = $true)][string]$Arch
    )

    $args = @("-S", ".", "-B", $BuildPath, "--fresh", "-G", $Candidate.Name)
    if ($Candidate.NeedsPlatform) {
        $args += @("-A", $Arch)
    }
    if (-not $Candidate.MultiConfig) {
        $args += "-DCMAKE_BUILD_TYPE=$ConfigName"
    }

    & cmake @args
    return ($LASTEXITCODE -eq 0)
}

function Invoke-Build {
    param(
        [Parameter(Mandatory = $true)][pscustomobject]$Candidate,
        [Parameter(Mandatory = $true)][string]$BuildPath,
        [Parameter(Mandatory = $true)][string]$ConfigName
    )

    $args = @("--build", $BuildPath)
    if ($Candidate.MultiConfig) {
        $args += @("--config", $ConfigName)
    }
    & cmake @args
    return ($LASTEXITCODE -eq 0)
}

function Invoke-Tests {
    param(
        [Parameter(Mandatory = $true)][pscustomobject]$Candidate,
        [Parameter(Mandatory = $true)][string]$BuildPath,
        [Parameter(Mandatory = $true)][string]$ConfigName
    )

    $args = @("--test-dir", $BuildPath, "--output-on-failure")
    if ($Candidate.MultiConfig) {
        $args += @("-C", $ConfigName)
    }
    & ctest @args
    return ($LASTEXITCODE -eq 0)
}

function Get-BuiltExePath {
    param(
        [Parameter(Mandatory = $true)][pscustomobject]$Candidate,
        [Parameter(Mandatory = $true)][string]$BuildPath,
        [Parameter(Mandatory = $true)][string]$ConfigName
    )

    if ($Candidate.MultiConfig) {
        return Join-Path $BuildPath "$ConfigName/technical-standard-note.exe"
    }
    return Join-Path $BuildPath "technical-standard-note.exe"
}

if (-not (Test-ToolAvailable "cmake")) {
    throw "CMake is not available in PATH."
}

$scriptDir = Split-Path -Parent $PSCommandPath
$repoRoot = Split-Path -Parent $scriptDir
Push-Location $repoRoot
try {
    $availableGenerators = Get-AvailableGenerators
    if ($availableGenerators.Count -eq 0) {
        throw "No CMake generators detected."
    }

    $allCandidates = @(
        (New-Candidate -Name "NMake Makefiles" -Id "nmake" -NeedsPlatform $false -MultiConfig $false -RequiredTools @("cl", "nmake")),
        (New-Candidate -Name "Visual Studio 18 2026" -Id "vs2026" -NeedsPlatform $true -MultiConfig $true -RequiredTools @()),
        (New-Candidate -Name "Visual Studio 17 2022" -Id "vs2022" -NeedsPlatform $true -MultiConfig $true -RequiredTools @()),
        (New-Candidate -Name "Ninja Multi-Config" -Id "ninja-multi" -NeedsPlatform $false -MultiConfig $true -RequiredTools @("ninja")),
        (New-Candidate -Name "Ninja" -Id "ninja" -NeedsPlatform $false -MultiConfig $false -RequiredTools @("ninja")),
        (New-Candidate -Name "MinGW Makefiles" -Id "mingw" -NeedsPlatform $false -MultiConfig $false -RequiredTools @("mingw32-make", "g++"))
    )

    $candidates = @()
    if ($ForceNMake) {
        $candidates += $allCandidates | Where-Object { $_.Name -eq "NMake Makefiles" }
    }
    elseif ($ForceNinja) {
        $candidates += $allCandidates | Where-Object { $_.Name -eq "Ninja Multi-Config" -or $_.Name -eq "Ninja" }
    }
    elseif ($ForceMinGW) {
        $candidates += $allCandidates | Where-Object { $_.Name -eq "MinGW Makefiles" }
    }
    else {
        $candidates = $allCandidates
    }

    $selected = $null
    $selectedBuildDir = ""

    foreach ($candidate in $candidates) {
        if ($candidate.Name -notin $availableGenerators) {
            continue
        }

        $missingTools = @()
        foreach ($tool in $candidate.RequiredTools) {
            if (-not (Test-ToolAvailable $tool)) {
                $missingTools += $tool
            }
        }
        if ($missingTools.Count -gt 0) {
            Write-Host ("[skip] {0} (missing tools: {1})" -f $candidate.Name, ($missingTools -join ", "))
            continue
        }

        $candidateBuildDir = Get-CandidateBuildDir -Candidate $candidate -ConfigName $Config -UserBuildDir $BuildDir
        Write-Host ("[try] configure with generator: {0}" -f $candidate.Name)
        if (-not (Invoke-Configure -Candidate $candidate -BuildPath $candidateBuildDir -ConfigName $Config -Arch $Platform)) {
            Write-Host ("[fail] configure with generator: {0}" -f $candidate.Name)
            continue
        }

        Write-Host ("[try] build with generator: {0}" -f $candidate.Name)
        if (-not (Invoke-Build -Candidate $candidate -BuildPath $candidateBuildDir -ConfigName $Config)) {
            Write-Host ("[fail] build with generator: {0}" -f $candidate.Name)
            continue
        }

        $candidateExePath = Get-BuiltExePath -Candidate $candidate -BuildPath $candidateBuildDir -ConfigName $Config
        if (-not (Test-Path -LiteralPath $candidateExePath)) {
            Write-Host ("[fail] build output missing for generator: {0}" -f $candidate.Name)
            continue
        }

        if ($RunTests) {
            Write-Host ("[run] tests with generator: {0}" -f $candidate.Name)
            if (-not (Invoke-Tests -Candidate $candidate -BuildPath $candidateBuildDir -ConfigName $Config)) {
                throw "Tests failed."
            }
        }

        $selected = $candidate
        $selectedBuildDir = $candidateBuildDir
        break
    }

    if (-not $selected) {
        throw "No usable generator/toolchain found. Use Developer PowerShell with MSVC tools or install MinGW/Ninja."
    }

    Write-Host ("[ok] selected generator: {0}" -f $selected.Name)
    Write-Host ("[ok] build directory: {0}" -f $selectedBuildDir)

    $exePath = Get-BuiltExePath -Candidate $selected -BuildPath $selectedBuildDir -ConfigName $Config
    if (Test-Path -LiteralPath $exePath) {
        Write-Host ("[ok] executable: {0}" -f (Resolve-Path -LiteralPath $exePath).Path)
    }
    else {
        Write-Host ("[warn] executable not found at expected path: {0}" -f $exePath)
    }

    if ($RunApp) {
        if (-not (Test-Path -LiteralPath $exePath)) {
            throw "Cannot run app because executable was not found: $exePath"
        }
        Write-Host "[run] launching app"
        Start-Process -FilePath $exePath | Out-Null
    }
}
finally {
    Pop-Location
}
