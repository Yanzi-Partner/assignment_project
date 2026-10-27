$ErrorActionPreference = "Stop"

$root = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $root

$gxx = (Get-Command g++ -ErrorAction SilentlyContinue).Source
if (-not $gxx) {
    $gxx = "E:\code\x86_64-15.2.0-release-win32-seh-ucrt-rt_v13-rev0\mingw64\bin\g++.exe"
}

# 收集所有源文件（src 目录下的 *.cpp）
$sources = @(Get-ChildItem -LiteralPath (Join-Path $root "src") -Filter "*.cpp" |
    ForEach-Object { $_.FullName })

$includeDir = Join-Path $root "include"
$output = Join-Path $root "assignment_project.exe"

# -Wall -Wextra -Wpedantic：打开常见告警
# -finput-charset / -fexec-charset：源码与运行期字符串统一 UTF-8，中文不乱码
& $gxx -std=c++17 -Wall -Wextra -Wpedantic -g -static `
    -finput-charset=UTF-8 -fexec-charset=UTF-8 `
    "-I$includeDir" -o $output @sources

if ($LASTEXITCODE -ne 0) {
    throw "编译失败，退出码：$LASTEXITCODE"
}

Write-Host "构建成功：$output"
