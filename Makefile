# Makefile for OS Course Design

CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lm -lpthread

# 目标文件
TARGETS = scheduler memory sync filesystem

# 默认目标
all: $(TARGETS)

# 处理机调度程序
scheduler: scheduler.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# 内存管理程序
memory: memory.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# 进程同步程序
sync: sync.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# 文件系统程序
filesystem: filesystem.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# 清理编译文件
clean:
	rm -f $(TARGETS) *.o

# 调试编译
debug: CFLAGS += -g
debug: clean $(TARGETS)

# 安装依赖（Ubuntu/Debian）
install-deps:
	sudo apt-get update
	sudo apt-get install -y build-essential gdb make

# 帮助信息
help:
	@echo "可用目标:"
	@echo "  make            - 编译所有程序"
	@echo "  make scheduler  - 编译处理机调度程序"
	@echo "  make memory     - 编译内存管理程序"
	@echo "  make sync       - 编译进程同步程序"
	@echo "  make filesystem - 编译文件系统程序"
	@echo "  make clean      - 清理编译文件"
	@echo "  make debug      - 调试模式编译"
	@echo "  make install-deps - 安装开发依赖"
	@echo "  make help       - 显示此帮助信息"

.PHONY: all clean debug install-deps help