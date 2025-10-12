@echo off
echo.
echo.
echo.
echo.
echo.
echo.
echo --------------------------------------------------------------------------------



REM see also:
REM https://vulkan.lunarg.com/doc/sdk

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



:_vulkan_end
echo --------------------------------------------------------------------------------
echo.
echo.
echo.
echo.
echo.
echo.
echo ===== Done.
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
