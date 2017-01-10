# Expect this script to be in the test subdirectory, moving up to cpp will make life easier
Push-Location "$PSScriptRoot/..";

# Running MSBuild requires that we find and load the location of MSBuild into PATH
if (!(Get-Command "MSBuild" -ErrorAction SilentlyContinue)) {
    #From http://stackoverflow.com/a/2124759/1118660
    #Set environment variables for Visual Studio Command Prompt
    Push-Location "$env:VS140COMNTOOLS";
    cmd /c "vsvars32.bat&set" |
    foreach {
      if ($_ -match "=") {
        $v = $_.split("=");
        set-item -force -path "ENV:\$($v[0])"  -value "$($v[1])"
      }
    }
    Pop-Location;
    Write-Host "Visual Studio 2015 Command Prompt variables set.`n" -ForegroundColor Yellow;
}

# Delete bin to make sure we get a clean build, and make sure the build Directory exists
if (Test-Path bin) {
    Remove-Item -Recurse -Force bin;
}
if (!(Test-Path build)) {
    New-Item build -ItemType Directory;
}
New-Item bin/testresults -ItemType Directory -Force;

Push-Location build;
Write-Host "Running CMake." -ForegroundColor Yellow;
cmake ..;
if (!($?)) {
    Write-Warning "CMake did not run correctly. Exiting.";
    Pop-Location;
    Pop-Location;
    return;
}
Write-Host "`nRunning MSBuild." -ForegroundColor Yellow;
# To build in release mode add /p:Configuration=Release
MSBuild /v:m /m GSAP.sln;
if (!($?)) {
    Write-Warning "MSBuild did not run correctly. Exiting.";
    Pop-Location;
    Pop-Location;
    return;
}
Pop-Location;

Push-Location bin;
Write-Host "`nRunning Tests." -ForegroundColor Yellow;
$failedTests = 0;

./supportTests.exe;
Write-Host "";
if ($lastExitCode -ne 0) {
    Write-Warning "$lastExitCode tests failed.`n`n`n";
}
$failedTests += $lastExitCode;

./frameworkTests.exe;
Write-Host "";
if ($lastExitCode -ne 0) {
    Write-Warning "$lastExitCode tests failed.`n`n`n";
}
$failedTests += $lastExitCode;

./commCollectionTests.exe;
Write-Host "";
if ($lastExitCode -ne 0) {
    Write-Warning "$lastExitCode tests failed.`n`n`n";
}
$failedTests += $lastExitCode;

./batteryPrognoserTests.exe;
Write-Host "";
if ($lastExitCode -ne 0) {
    Write-Warning "$lastExitCode tests failed.`n`n`n";
}
$failedTests += $lastExitCode;

Write-Host "`n${failedTests} tests failed." -ForegroundColor Yellow;
Pop-Location;

Pop-Location;
