@echo off 
echo.
echo.
color 0A
echo                        Cleanup Tool for VisualStudio
echo                               Copyright (C)  2009
echo                          S.Orlando   Nov. 25th 2009
echo.
echo.
echo    正在搜索并删除obj文件.....
for /f "delims=" %%i in ('dir /b /a-d /s "*.obj"') do del "%%i"
echo    删除obj文件完毕
echo.
echo   正在搜索并删除pch文件...
for /f "delims=" %%i in ('dir /b /a-d /s "*.pch"') do del "%%i"
echo   删除pch文件完毕
echo.
echo   正在搜索并删除pdb文件...
for /f "delims=" %%i in ('dir /b /a-d /s "*.pdb"') do del "%%i"
echo   删除pdb文件完毕
echo.
echo   正在搜索并删除idb文件...
for /f "delims=" %%i in ('dir /b /a-d /s "*.idb"') do del "%%i"
echo   删除idb文件完毕
echo.
echo   正在搜索并删除dep文件...
for /f "delims=" %%i in ('dir /b /a-d /s "*.dep"') do del "%%i"
echo   删除dep文件完毕
echo.
echo   正在搜索并删除sbr文件...
for /f "delims=" %%i in ('dir /b /a-d /s "*.sbr"') do del "%%i"
echo   删除sbr文件完毕
echo.
echo   正在搜索并删除ncb文件...
for /f "delims=" %%i in ('dir /b /a-d /s "*.ncb"') do del "%%i"
echo   删除ncb文件完毕
echo.
echo   正在搜索并删除bsc文件...
for /f "delims=" %%i in ('dir /b /a-d /s "*.bsc"') do del "%%i"
echo   删除ncb文件完毕
rem ===============vs 2010
echo.
echo   正在搜索并删除ipch文件...
for /f "delims=" %%i in ('dir /b /a-d /s "*.ipch"') do del "%%i"
echo   删除ipch文件完毕
echo.
echo   正在搜索并删除sdf文件...
for /f "delims=" %%i in ('dir /b /a-d /s "*.sdf"') do del "%%i"
echo   删除sdf文件完毕
echo.
echo   正在搜索并删除tlog文件...
for /f "delims=" %%i in ('dir /b /a-d /s "*.tlog"') do del "%%i"
echo   删除tlog文件完毕
echo.
echo   正在搜索并删除ilk文件...
for /f "delims=" %%i in ('dir /b /a-d /s "*.ilk"') do del "%%i"
echo   删除ilk文件完毕



echo.
