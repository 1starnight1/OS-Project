#!/bin/bash

# 操作系统课程设计 - 测试脚本

echo "========================================="
echo "  操作系统课程设计 - 自动测试"
echo "========================================="
echo ""

# 颜色定义
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 测试计数器
TOTAL_TESTS=0
PASSED_TESTS=0

# 测试函数
test_program() {
    local program=$1
    local test_name=$2
    local input=$3
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    echo "测试 $TOTAL_TESTS: $test_name"
    
    if [ -f "$program" ]; then
        if [ -n "$input" ]; then
            echo "$input" | timeout 5 ./$program > /dev/null 2>&1
        else
            timeout 5 ./$program > /dev/null 2>&1 << EOF
1
exit
EOF
        fi
        
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}✓ 通过${NC}"
            PASSED_TESTS=$((PASSED_TESTS + 1))
        else
            echo -e "${RED}✗ 失败${NC}"
        fi
    else
        echo -e "${RED}✗ 程序不存在${NC}"
    fi
    
    echo ""
}

# 检查程序是否存在
check_programs() {
    echo "检查可执行程序..."
    echo "----------------------------------------"
    
    for program in scheduler memory sync filesystem; do
        if [ -f "$program" ]; then
            echo -e "${GREEN}✓${NC} $program 存在"
        else
            echo -e "${RED}✗${NC} $program 不存在"
        fi
    done
    
    echo "----------------------------------------"
    echo ""
}

# 运行测试
run_tests() {
    echo "开始运行测试..."
    echo "----------------------------------------"
    echo ""
    
    # 测试处理机调度
    test_program "scheduler" "处理机调度算法" "3
0 5 2
1 3 1
2 1 3
2
exit"
    
    # 测试内存管理
    test_program "memory" "内存管理" "1
1 100
2 200
-1
1
-1
5"
    
    # 测试进程同步
    test_program "sync" "进程同步" "1"
    
    # 测试文件系统
    test_program "filesystem" "文件系统" "mkdir test
touch file.txt 50
ls
rm file.txt
exit"
    
    echo "----------------------------------------"
    echo ""
}

# 显示测试结果
show_results() {
    echo "========================================="
    echo "  测试结果汇总"
    echo "========================================="
    echo ""
    echo "总测试数: $TOTAL_TESTS"
    echo -e "通过: ${GREEN}$PASSED_TESTS${NC}"
    echo -e "失败: ${RED}$((TOTAL_TESTS - PASSED_TESTS))${NC}"
    
    if [ $PASSED_TESTS -eq $TOTAL_TESTS ]; then
        echo ""
        echo -e "${GREEN}所有测试通过！${NC}"
    else
        echo ""
        echo -e "${YELLOW}部分测试失败，请检查程序实现${NC}"
    fi
    
    echo ""
}

# 手动测试菜单
manual_test() {
    echo "========================================="
    echo "  手动测试菜单"
    echo "========================================="
    echo ""
    echo "1. 测试处理机调度算法"
    echo "2. 测试内存管理"
    echo "3. 测试进程同步"
    echo "4. 测试文件系统"
    echo "5. 运行所有测试"
    echo "6. 退出"
    echo ""
    
    read -p "请选择: " choice
    
    case $choice in
        1)
            echo "启动处理机调度算法测试..."
            ./scheduler
            ;;
        2)
            echo "启动内存管理测试..."
            ./memory
            ;;
        3)
            echo "启动进程同步测试..."
            ./sync
            ;;
        4)
            echo "启动文件系统测试..."
            ./filesystem
            ;;
        5)
            check_programs
            run_tests
            show_results
            ;;
        6)
            echo "退出测试"
            exit 0
            ;;
        *)
            echo "无效选择"
            ;;
    esac
}

# 主函数
main() {
    if [ "$1" == "--auto" ]; then
        check_programs
        run_tests
        show_results
    elif [ "$1" == "--manual" ]; then
        manual_test
    else
        echo "用法: $0 [选项]"
        echo ""
        echo "选项:"
        echo "  --auto     自动运行所有测试"
        echo "  --manual   手动选择测试"
        echo "  --help     显示帮助信息"
        echo ""
        echo "示例:"
        echo "  $0 --auto     # 自动测试所有程序"
        echo "  $0 --manual   # 手动选择测试"
    fi
}

main "$@"