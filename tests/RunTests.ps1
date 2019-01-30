# Expect this script to be in the test subdirectory
Push-Location "$PSScriptRoot";

# Delete bin to make sure we get a clean build, and make sure the build Directory exists
if (!(Test-Path build)) {
    New-Item build -ItemType Directory;
}

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
cmake --build . --clean-first -- /p:Configuration=Release;
if (!($?)) {
    Write-Warning "MSBuild did not run correctly. Exiting.";
    Pop-Location;
    Pop-Location;
    return;
}

Push-Location bin;
Write-Host "`nRunning Tests." -ForegroundColor Yellow;

./tests.exe;
Write-Host "";
if ($lastExitCode -ne 0) {
    Write-Warning "$lastExitCode tests failed.`n`n`n";
}
$failedTests = $lastExitCode;

Write-Host "`n${failedTests} tests failed." -ForegroundColor Yellow;
Pop-Location;

Pop-Location;

exit $failedTests;
