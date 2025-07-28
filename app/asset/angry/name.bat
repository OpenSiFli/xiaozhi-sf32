@echo off
setlocal enabledelayedexpansion

:: 获取用户输入的基础文件名
set /p "baseName=please enter the base file name(no spaces):"

:: 检查输入是否包含空格
echo !baseName! | findstr /r " " >nul
if not errorlevel 1 (
    echo error: file name cannot contain spaces
    pause
    exit /b 1
)

:: 初始化计数器
set count=1

:: 遍历当前目录下的所有文件，按顺序重命名（排除脚本自身）
for /f "delims=" %%f in ('dir /b /a-d ^| findstr /v /i "%~nx0"') do (
    :: 获取文件扩展名
    set "ext=%%~xf"
    :: 重命名文件为 基础名_序号.扩展名
    ren "%%f" "!baseName!_!count!!ext!"
    set /a count+=1
)

echo rename complete!
pause