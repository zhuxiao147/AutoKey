@echo off
echo 检查开发环境配置...
echo =========================

echo.
echo 1. 检查Qt环境:
where qmake >nul 2>nul
if %errorlevel% equ 0 (
    echo [OK] qmake 已找到
    qmake -v
) else (
    echo [ERROR] qmake 未找到，请确保Qt已正确安装并配置环境变量
)

echo.
echo 2. 检查编译器:
where cl >nul 2>nul
if %errorlevel% equ 0 (
    echo [OK] MSVC 编译器已找到
    cl 2>&1 | findstr /C:"Version"
) else (
    echo [WARNING] MSVC 编译器未找到，尝试检查MinGW...
    where g++ >nul 2>nul
    if %errorlevel% equ 0 (
        echo [OK] MinGW 编译器已找到
        g++ --version
    ) else (
        echo [ERROR] 未找到任何C++编译器
    )
)

echo.
echo 3. 检查Windows SDK:
if exist "%WindowsSdkDir%" (
    echo [OK] Windows SDK 已安装
    echo SDK路径: %WindowsSdkDir%
) else (
    echo [WARNING] Windows SDK 未找到，HID功能可能受限
)

echo.
echo 4. 检查项目文件:
if exist "AutoKeyTool.pro" (
    echo [OK] 项目文件存在
) else (
    echo [ERROR] 项目文件丢失
)

echo.
echo 环境检查完成！
echo.
echo 如果所有检查都通过，可以运行 build.bat 开始构建项目。
pause