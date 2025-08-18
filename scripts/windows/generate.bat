@echo off

echo ===== Generating Visual Studio 2022 workspace
pushd ..\..\
call vendor\dev-tools\premake\windows\premake5.exe vs2022
popd

echo.
echo.
echo.
echo.
echo.
echo.
echo ===== Done.
pause
