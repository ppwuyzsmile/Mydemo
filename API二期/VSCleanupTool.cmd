@echo off 
echo.
echo.
color 0A
echo                        Cleanup Tool for VisualStudio
echo                               Copyright (C)  2009
echo                          S.Orlando   Nov. 25th 2009
echo.
echo.
echo    ����������ɾ��obj�ļ�.....
for /f "delims=" %%i in ('dir /b /a-d /s "*.obj"') do del "%%i"
echo    ɾ��obj�ļ����
echo.
echo   ����������ɾ��pch�ļ�...
for /f "delims=" %%i in ('dir /b /a-d /s "*.pch"') do del "%%i"
echo   ɾ��pch�ļ����
echo.
echo   ����������ɾ��pdb�ļ�...
for /f "delims=" %%i in ('dir /b /a-d /s "*.pdb"') do del "%%i"
echo   ɾ��pdb�ļ����
echo.
echo   ����������ɾ��idb�ļ�...
for /f "delims=" %%i in ('dir /b /a-d /s "*.idb"') do del "%%i"
echo   ɾ��idb�ļ����
echo.
echo   ����������ɾ��dep�ļ�...
for /f "delims=" %%i in ('dir /b /a-d /s "*.dep"') do del "%%i"
echo   ɾ��dep�ļ����
echo.
echo   ����������ɾ��sbr�ļ�...
for /f "delims=" %%i in ('dir /b /a-d /s "*.sbr"') do del "%%i"
echo   ɾ��sbr�ļ����
echo.
echo   ����������ɾ��ncb�ļ�...
for /f "delims=" %%i in ('dir /b /a-d /s "*.ncb"') do del "%%i"
echo   ɾ��ncb�ļ����
echo.
echo   ����������ɾ��bsc�ļ�...
for /f "delims=" %%i in ('dir /b /a-d /s "*.bsc"') do del "%%i"
echo   ɾ��ncb�ļ����
rem ===============vs 2010
echo.
echo   ����������ɾ��ipch�ļ�...
for /f "delims=" %%i in ('dir /b /a-d /s "*.ipch"') do del "%%i"
echo   ɾ��ipch�ļ����
echo.
echo   ����������ɾ��sdf�ļ�...
for /f "delims=" %%i in ('dir /b /a-d /s "*.sdf"') do del "%%i"
echo   ɾ��sdf�ļ����
echo.
echo   ����������ɾ��tlog�ļ�...
for /f "delims=" %%i in ('dir /b /a-d /s "*.tlog"') do del "%%i"
echo   ɾ��tlog�ļ����
echo.
echo   ����������ɾ��ilk�ļ�...
for /f "delims=" %%i in ('dir /b /a-d /s "*.ilk"') do del "%%i"
echo   ɾ��ilk�ļ����



echo.
