@echo off
echo.
echo.
echo.
echo.
echo.
echo.
echo --------------------------------------------------------------------------------



set VER=5.0.0-beta7
set URL=https://github.com/premake/premake-core/releases/download/v%VER%/premake-%VER%-windows.zip
set TARGET=..\..\vendor\dev-tools\premake\windows
mkdir %TARGET% >NUL 2>&1

echo ===== Download premake v%VER% from GitHub
powershell -Command "Invoke-WebRequest %URL% -OutFile %TARGET%\premake-%VER%-windows.zip"
if %ERRORLEVEL% == 0 goto _premake_extract
echo ERROR: Error during download!
goto _error_exit

:_premake_extract
echo ===== Extract premake archive
powershell -Command "Expand-Archive %TARGET%\premake-%VER%-windows.zip -DestinationPath %TARGET% -Force"
if %ERRORLEVEL% == 0 goto _premake_test
echo ERROR: Error during extraction!
goto _error_exit

:_premake_test
echo ===== Check for premake [premake5.exe --version]
%TARGET%\premake5.exe --version
if %ERRORLEVEL% == 0 goto _premake_end
echo ERROR: Error during test!
goto _error_exit



:_premake_end
echo --------------------------------------------------------------------------------



REM see also:
REM https://vulkan.lunarg.com/doc/sdk
if not %VULKAN_SDK% == "" goto _vulkan_skip

set TARGET=..\..\vendor\dev-tools\vulkan-sdk\windows
set URL=https://sdk.lunarg.com/sdk/download/latest/windows/vulkan-sdk.exe
mkdir %TARGET% >NUL 2>&1

echo ===== Download Vulkan SDK (~250MB)
if not exist %TARGET%\vulkan-sdk-install.exe (
	echo Note: This will take a while
	powershell -Command "Invoke-WebRequest %URL% -OutFile %TARGET%\vulkan-sdk-install.exe"
) else (
	echo Note: Found previous download (skipping)
	set ERRORLEVEL=0
)
if %ERRORLEVEL% == 0 goto _vulkan_install
echo ERROR: Error during download!
goto _error_exit

:_vulkan_install
echo ===== Install Vulkan SDK ...
start /WAIT %TARGET%\vulkan-sdk-install.exe
if %ERRORLEVEL% == 0 goto _vulkan_end
echo ERROR: Error during installation!
goto _error_exit



goto _vulkan_end
:_vulkan_skip
echo ===== Download/Install Vulkan SDK skipped!
echo Call "update-vulkan.bat" to force updating the Vulkan SDK.
:_vulkan_end
echo --------------------------------------------------------------------------------



echo ===== Check for git [git --version]
git --version
if %ERRORLEVEL% == 0 goto _git_run
echo ERROR: Error, git not found!
goto _error_exit

:_git_run
echo ===== Fetching vendor repos
pushd ..\..\
git submodule update --init --recursive
popd
if %ERRORLEVEL% == 0 goto _git_end
echo ERROR: Error during fetching!
goto _error_exit



:_git_end
echo --------------------------------------------------------------------------------
call generate.bat
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
