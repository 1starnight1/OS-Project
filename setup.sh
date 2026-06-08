#!/bin/bash

# 操作系统课程设计 - 快速开始脚本

echo "========================================="
echo "  操作系统课程设计 - 快速开始指南"
echo "========================================="
echo ""

# 检查是否在Linux环境中
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    echo "警告: 此脚本需要在Linux环境中运行"
    echo "请在Ubuntu虚拟机中运行此脚本"
    exit 1
fi

# 检查GCC是否安装
if ! command -v gcc &> /dev/null; then
    echo "GCC未安装，正在安装..."
    sudo apt-get update
    sudo apt-get install -y build-essential
fi

# 检查Make是否安装
if ! command -v make &> /dev/null; then
    echo "Make未安装，正在安装..."
    sudo apt-get install -y make
fi

echo ""
echo "========================================="
echo "  开始编译程序..."
echo "========================================="
echo ""

# 编译所有程序
make clean
make

if [ $? -eq 0 ]; then
    echo ""
    echo "========================================="
    echo "  编译成功！"
    echo "========================================="
    echo ""
    echo "可执行程序:"
    ls -lh scheduler memory sync filesystem
    echo ""
    echo "运行示例:"
    echo "  ./scheduler    - 处理机调度算法"
    echo "  ./memory       - 内存管理与页面置换"
    echo "  ./sync         - 进程同步与并发控制"
    echo "  ./filesystem   - 文件系统模拟"
    echo ""
    echo "测试程序:"
    echo "  make test      - 运行所有测试"
    echo ""
else
    echo ""
    echo "编译失败，请检查错误信息"
    exit 1
fi