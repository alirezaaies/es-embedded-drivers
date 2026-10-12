@echo off

python "%~dp0sync_keil.py"
if errorlevel 1 exit /b %errorlevel%
python "%~dp0check_project.py"

exit /b %errorlevel%
