@echo off
echo 正在构建自动按键设置工具...

where qmake >nul 2>nul
if %errorlevel% neq 0 (
    echo 错误：未找到qmake，请确保Qt已正确安装并配置环境变量
    exit /b 1
)

echo 清理旧的构建文件...
if exist build rmdir /s /q build
mkdir build
cd build

echo 生成Makefile...
qmake ..\AutoKeyTool.pro
if %errorlevel% neq 0 (
    echo 错误：qmake失败
    exit /b 1
)

echo 编译项目...
nmake
if %errorlevel% neq 0 (
    echo 错误：编译失败
    exit /b 1
)

echo 构建完成！
echo 可执行文件位置：build\release\AutoKeyTool.exe
cd ..
pause