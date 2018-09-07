# Expect this script to be in the test subdirectory, moving up to cpp will make life easier
Push-Location "$PSScriptRoot/..";

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
cmake --build .;
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

./gsapTests.exe;
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

Write-Host "`n${failedTests} tests failed." -ForegroundColor Yellow;
Pop-Location;

Pop-Location;
