@echo off

echo ===== Cleanup all Visual Studio Files...
pushd ..\..\

rmdir /q /s .vs
for /r %%i in (*vcxproj*) do ( 
	del /f /q "%%i" 
)
del /f /q "*.sln" 

popd

echo ===== Done.
pause
