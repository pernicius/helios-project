@echo off
echo.
echo.
echo.
echo.
echo.
echo.
echo --------------------------------------------------------------------------------



set VER=5.0.0-beta7
set DLPATH=https://github.com/premake/premake-core/releases/download/v%VER%/premake-%VER%-windows.zip
set DSTPATH=..\..\vendor\dev-tools\premake\windows
mkdir %DSTPATH% >NUL 2>&1
set ERRORLEVEL=0

echo ===== Download premake v%VER% from GitHub ...
powershell -Command "Invoke-WebRequest %DLPATH% -OutFile %DSTPATH%\premake-%VER%-windows.zip"

if %ERRORLEVEL% == 0 goto _premake_extract
echo ERROR: Error during download!
goto _error_exit

:_premake_extract
echo ===== Extract premake archive ...
powershell -Command "Expand-Archive %DSTPATH%\premake-%VER%-windows.zip -DestinationPath %DSTPATH% -Force"

if %ERRORLEVEL% == 0 goto _premake_test
echo ERROR: Error during extraction!
goto _error_exit

:_premake_test
echo ===== Check for premake [premake5.exe --version] ...
%DSTPATH%\premake5.exe --version

if %ERRORLEVEL% == 0 goto _premake_end
echo ERROR: Error during test!
goto _error_exit



:_premake_end
echo --------------------------------------------------------------------------------



set DSTPATH=..\..\vendor\dev-tools\vulkan-sdk\windows
mkdir %DSTPATH% >NUL 2>&1
set ERRORLEVEL=0

REM see also:
REM https://vulkan.lunarg.com/doc/sdk

echo ===== Download Vulkan SDK (~250MB) ...
echo Note: This will take a while...
rem powershell -Command "Invoke-WebRequest https://sdk.lunarg.com/sdk/download/latest/windows/vulkan-sdk.exe -OutFile %DSTPATH%\vulkan-sdk-install.exe"

rem if %ERRORLEVEL% == 0 goto _vulkan_install
rem echo ERROR: Error during download!
rem goto _error_exit

:_vulkan_install
echo ===== Install Vulkan SDK ...
rem start /WAIT %DSTPATH%\vulkan-sdk-install.exe

if %ERRORLEVEL% == 0 goto _vulkan_end
echo ERROR: Error during installation!
goto _error_exit



:_vulkan_end
echo --------------------------------------------------------------------------------



echo ===== Check for git [git --version] ...
git --version

if %ERRORLEVEL% == 0 goto _git_run
echo ERROR: Error, git not found!
goto _error_exit

:_git_run
echo ===== Fetching vendor repos ...

if %ERRORLEVEL% == 0 goto _git_end
echo ERROR: Error during fetching!
goto _error_exit



:_git_end
echo --------------------------------------------------------------------------------
echo.
echo.
echo.
echo.
echo.
echo.
echo ===== Done!
pause
exit 0
:_error_exit
echo --------------------------------------------------------------------------------
echo.
echo.
echo.
echo.
echo.
echo.
echo ===== Error! (%ERRORLEVEL%)
pause
exit 1
