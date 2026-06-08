# 快速使用指南

## 🚀 在Ubuntu虚拟机中使用

### 第一步：传输文件到虚拟机

将 `linux_os` 文件夹传输到你的Ubuntu虚拟机中，可以使用以下方法：

**方法1：使用共享文件夹**
1. 在VMware中设置共享文件夹
2. 在Ubuntu中挂载共享文件夹：`sudo mount -t vboxsf shared_folder /mnt/shared`

**方法2：使用SCP传输**
```bash
# 在Windows PowerShell中执行
scp -r "C:\Users\Lenovo\Desktop\OS大作业\linux_os" username@vm_ip:/home/username/
```

**方法3：直接复制粘贴**
1. 在Windows中复制代码内容
2. 在Ubuntu终端中使用文本编辑器粘贴保存

### 第二步：进入项目目录

```bash
cd ~/linux_os
```

### 第三步：编译程序

**方法1：使用自动脚本（推荐）**
```bash
chmod +x setup.sh
./setup.sh
```

**方法2：使用Makefile**
```bash
make
```

**方法3：手动编译**
```bash
gcc -o scheduler scheduler.c -lm
gcc -o memory memory.c -lm
gcc -o sync sync.c -lpthread
gcc -o filesystem filesystem.c -lm
```

### 第四步：运行程序

```bash
# 处理机调度算法
./scheduler

# 内存管理
./memory

# 进程同步
./sync

# 文件系统
./filesystem
```

### 第五步：测试程序

```bash
chmod +x test.sh
./test.sh --auto      # 自动测试所有程序
./test.sh --manual    # 手动选择测试
```

## 📝 各程序使用示例

### 1. 处理机调度算法

```bash
./scheduler
```

**输入示例：**
```
请输入进程数量: 4

请输入进程信息 (到达时间 执行时间 优先级):
P1: 0 5 2
P2: 1 3 1
P3: 2 1 3
P4: 4 6 2

请输入时间片大小: 2
```

### 2. 内存管理

```bash
./memory
```

**选择功能：**
```
请选择功能:
1. 首次适应算法 (First Fit)
2. 最佳适应算法 (Best Fit)
3. FIFO 页面置换
4. LRU 页面置换
```

### 3. 进程同步

```bash
./sync
```

**选择要模拟的问题：**
```
请选择要模拟的问题:
1. 生产者-消费者问题
2. 读者-写者问题
3. 哲学家进餐问题
```

### 4. 文件系统

```bash
./filesystem
```

**常用命令：**
```
/ $ mkdir documents
/ $ cd documents
/documents $ touch report.txt 100
/documents $ ls
/documents $ info report.txt
/documents $ cd ..
/ $ df
/ $ exit
```

## 🔧 常见问题解决

### 编译错误

**问题：找不到pthread库**
```bash
sudo apt-get install build-essential
```

**问题：权限不足**
```bash
chmod +x scheduler memory sync filesystem setup.sh test.sh
```

### 运行时错误

**问题：程序无法运行**
```bash
# 检查文件权限
ls -l scheduler

# 添加执行权限
chmod +x scheduler
```

**问题：线程同步问题**
- 确保在Linux环境下运行
- 检查pthread库是否正确链接

## 📊 实验报告数据收集

### 收集调度算法结果

运行 `./scheduler` 后，记录以下数据：
- 各算法的平均周转时间
- 各算法的平均等待时间
- 调度顺序和甘特图

### 收集内存管理结果

运行 `./memory` 后，记录：
- 内存分配过程
- 缺页次数和缺页率
- 不同算法的性能对比

### 收集进程同步结果

运行 `./sync` 后，记录：
- 各同步问题的执行过程
- 线程同步的正确性
- 死锁避免机制

### 收集文件系统结果

运行 `./filesystem` 后，记录：
- 文件操作过程
- 磁盘空间使用情况
- 目录结构变化

## 🎯 下一步操作

1. **完成基础测试**：确保所有程序正常运行
2. **收集实验数据**：记录各种算法的性能指标
3. **撰写实验报告**：按照课程要求撰写报告
4. **代码托管**：将代码上传到GitHub
5. **准备提交**：6月11日前提交报告和代码

## 💡 提示

- 建议在虚拟机中使用 `tmux` 或 `screen` 来管理多个终端窗口
- 使用 `gdb` 调试程序：`gdb ./scheduler`
- 查看程序帮助：`./scheduler` (大多数程序都有交互式菜单)
- 定期保存实验数据，避免丢失

---

**祝你课程设计顺利完成！** 🎉