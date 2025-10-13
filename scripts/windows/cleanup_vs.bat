@echo off

echo ===== Cleanup all Visual Studio Files...
pushd ..\..\

echo DelDir:  .vs
rmdir /q /s .vs
for /r %%i in (*vcxproj*) do (
	echo DelFile: %%i
	del /f /q "%%i" 
)
echo DelFile: *.sln
del /f /q "*.sln" 

popd

echo ===== Done.
pause
