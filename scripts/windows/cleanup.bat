@echo off

echo ===== Cleanup...
pushd ..\..\
rmdir /q /s _bin
rmdir /q /s _build
popd

REM TODO: remove Visual Studio Files?

echo ===== Done.
pause
