#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_FILES 100
#define MAX_NAME_LENGTH 50
#define DISK_SIZE 1024

typedef struct {
    char name[MAX_NAME_LENGTH];
    int size;
    int is_directory;
    int start_block;
    int parent_index;
    time_t create_time;
    time_t modify_time;
} File;

typedef struct {
    int is_free;
    int file_index;
} DiskBlock;

File files[MAX_FILES];
DiskBlock disk[DISK_SIZE];
int file_count = 0;
int current_directory = -1; // -1 表示根目录

void init_filesystem() {
    file_count = 0;
    
    // 创建根目录
    strcpy(files[0].name, "/");
    files[0].size = 0;
    files[0].is_directory = 1;
    files[0].start_block = -1;
    files[0].parent_index = -1;
    files[0].create_time = time(NULL);
    files[0].modify_time = time(NULL);
    file_count = 1;
    
    // 初始化磁盘块
    for (int i = 0; i < DISK_SIZE; i++) {
        disk[i].is_free = 1;
        disk[i].file_index = -1;
    }
    
    current_directory = 0;
}

int find_file(const char *name, int parent_index) {
    for (int i = 0; i < file_count; i++) {
        if (files[i].parent_index == parent_index && 
            strcmp(files[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void print_current_path() {
    if (current_directory == 0) {
        printf("/");
        return;
    }
    
    int path[MAX_FILES];
    int path_length = 0;
    int current = current_directory;
    
    while (current != 0) {
        path[path_length++] = current;
        current = files[current].parent_index;
    }
    
    for (int i = path_length - 1; i >= 0; i--) {
        printf("/%s", files[path[i]].name);
    }
}

void list_files() {
    printf("\n当前目录: ");
    print_current_path();
    printf("\n");
    printf("----------------------------------------\n");
    printf("类型\t名称\t\t大小\t创建时间\n");
    printf("----------------------------------------\n");
    
    for (int i = 0; i < file_count; i++) {
        if (files[i].parent_index == current_directory) {
            struct tm *time_info = localtime(&files[i].create_time);
            char time_str[20];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", time_info);
            
            printf("%s\t%-15s\t%d\t%s\n",
                   files[i].is_directory ? "目录" : "文件",
                   files[i].name,
                   files[i].size,
                   time_str);
        }
    }
    printf("----------------------------------------\n");
}

void create_file(const char *name, int size) {
    if (find_file(name, current_directory) != -1) {
        printf("错误: 文件 '%s' 已存在\n", name);
        return;
    }
    
    if (file_count >= MAX_FILES) {
        printf("错误: 文件系统已满\n");
        return;
    }
    
    // 检查是否有足够的连续空间
    int start_block = -1;
    int consecutive_blocks = 0;
    
    for (int i = 0; i < DISK_SIZE; i++) {
        if (disk[i].is_free) {
            if (consecutive_blocks == 0) {
                start_block = i;
            }
            consecutive_blocks++;
            
            if (consecutive_blocks >= size) {
                break;
            }
        } else {
            consecutive_blocks = 0;
            start_block = -1;
        }
    }
    
    if (consecutive_blocks < size) {
        printf("错误: 磁盘空间不足\n");
        return;
    }
    
    // 分配磁盘块
    for (int i = start_block; i < start_block + size; i++) {
        disk[i].is_free = 0;
        disk[i].file_index = file_count;
    }
    
    // 创建文件
    strcpy(files[file_count].name, name);
    files[file_count].size = size;
    files[file_count].is_directory = 0;
    files[file_count].start_block = start_block;
    files[file_count].parent_index = current_directory;
    files[file_count].create_time = time(NULL);
    files[file_count].modify_time = time(NULL);
    
    printf("文件 '%s' 创建成功，大小: %d 字节\n", name, size);
    file_count++;
}

void create_directory(const char *name) {
    if (find_file(name, current_directory) != -1) {
        printf("错误: 目录 '%s' 已存在\n", name);
        return;
    }
    
    if (file_count >= MAX_FILES) {
        printf("错误: 文件系统已满\n");
        return;
    }
    
    strcpy(files[file_count].name, name);
    files[file_count].size = 0;
    files[file_count].is_directory = 1;
    files[file_count].start_block = -1;
    files[file_count].parent_index = current_directory;
    files[file_count].create_time = time(NULL);
    files[file_count].modify_time = time(NULL);
    
    printf("目录 '%s' 创建成功\n", name);
    file_count++;
}

void delete_file(const char *name) {
    int file_index = find_file(name, current_directory);
    
    if (file_index == -1) {
        printf("错误: 文件 '%s' 不存在\n", name);
        return;
    }
    
    if (files[file_index].is_directory) {
        // 检查目录是否为空
        int has_children = 0;
        for (int i = 0; i < file_count; i++) {
            if (files[i].parent_index == file_index) {
                has_children = 1;
                break;
            }
        }
        
        if (has_children) {
            printf("错误: 目录 '%s' 不为空\n", name);
            return;
        }
    } else {
        // 释放磁盘块
        for (int i = files[file_index].start_block; 
             i < files[file_index].start_block + files[file_index].size; i++) {
            disk[i].is_free = 1;
            disk[i].file_index = -1;
        }
    }
    
    // 删除文件记录
    for (int i = file_index; i < file_count - 1; i++) {
        files[i] = files[i + 1];
    }
    
    // 更新磁盘块中的文件索引
    for (int i = 0; i < DISK_SIZE; i++) {
        if (disk[i].file_index > file_index) {
            disk[i].file_index--;
        }
    }
    
    file_count--;
    printf("'%s' 删除成功\n", name);
}

void change_directory(const char *name) {
    if (strcmp(name, "..") == 0) {
        if (current_directory != 0) {
            current_directory = files[current_directory].parent_index;
        }
        return;
    }
    
    if (strcmp(name, "/") == 0) {
        current_directory = 0;
        return;
    }
    
    int dir_index = find_file(name, current_directory);
    
    if (dir_index == -1) {
        printf("错误: 目录 '%s' 不存在\n", name);
        return;
    }
    
    if (!files[dir_index].is_directory) {
        printf("错误: '%s' 不是目录\n", name);
        return;
    }
    
    current_directory = dir_index;
}

void print_disk_usage() {
    int used_blocks = 0;
    int free_blocks = 0;
    
    for (int i = 0; i < DISK_SIZE; i++) {
        if (disk[i].is_free) {
            free_blocks++;
        } else {
            used_blocks++;
        }
    }
    
    printf("\n磁盘使用情况:\n");
    printf("----------------------------------------\n");
    printf("总容量: %d 字节\n", DISK_SIZE);
    printf("已使用: %d 字节 (%.1f%%)\n", used_blocks, (float)used_blocks / DISK_SIZE * 100);
    printf("空闲: %d 字节 (%.1f%%)\n", free_blocks, (float)free_blocks / DISK_SIZE * 100);
    printf("----------------------------------------\n");
}

void print_file_info(const char *name) {
    int file_index = find_file(name, current_directory);
    
    if (file_index == -1) {
        printf("错误: 文件 '%s' 不存在\n", name);
        return;
    }
    
    struct tm *create_time = localtime(&files[file_index].create_time);
    struct tm *modify_time = localtime(&files[file_index].modify_time);
    char create_str[20], modify_str[20];
    strftime(create_str, sizeof(create_str), "%Y-%m-%d %H:%M", create_time);
    strftime(modify_str, sizeof(modify_str), "%Y-%m-%d %H:%M", modify_time);
    
    printf("\n文件信息: %s\n", name);
    printf("----------------------------------------\n");
    printf("类型: %s\n", files[file_index].is_directory ? "目录" : "文件");
    printf("大小: %d 字节\n", files[file_index].size);
    printf("创建时间: %s\n", create_str);
    printf("修改时间: %s\n", modify_str);
    if (!files[file_index].is_directory) {
        printf("起始块: %d\n", files[file_index].start_block);
    }
    printf("----------------------------------------\n");
}

int main() {
    init_filesystem();
    
    printf("========================================\n");
    printf("       简易文件系统模拟器\n");
    printf("========================================\n\n");
    
    printf("可用命令:\n");
    printf("  ls              - 列出当前目录内容\n");
    printf("  mkdir <name>    - 创建目录\n");
    printf("  touch <name> <size> - 创建文件\n");
    printf("  rm <name>       - 删除文件或目录\n");
    printf("  cd <name>       - 切换目录 (支持 .. 和 /)\n");
    printf("  pwd             - 显示当前路径\n");
    printf("  info <name>     - 显示文件详细信息\n");
    printf("  df              - 显示磁盘使用情况\n");
    printf("  help            - 显示帮助信息\n");
    printf("  exit            - 退出程序\n\n");
    
    char command[100];
    char arg1[50];
    char arg2[50];
    
    while (1) {
        printf("\n");
        print_current_path();
        printf(" $ ");
        
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }
        
        // 去除换行符
        command[strcspn(command, "\n")] = 0;
        
        if (strlen(command) == 0) {
            continue;
        }
        
        // 解析命令
        int argc = sscanf(command, "%s %s %s", arg1, arg2, command);
        
        if (argc == 0) {
            continue;
        }
        
        if (strcmp(arg1, "exit") == 0) {
            printf("程序结束\n");
            break;
        } else if (strcmp(arg1, "ls") == 0) {
            list_files();
        } else if (strcmp(arg1, "mkdir") == 0) {
            if (argc >= 2) {
                create_directory(arg2);
            } else {
                printf("用法: mkdir <目录名>\n");
            }
        } else if (strcmp(arg1, "touch") == 0) {
            if (argc >= 3) {
                int size = atoi(command);
                if (size > 0) {
                    create_file(arg2, size);
                } else {
                    printf("错误: 文件大小必须大于0\n");
                }
            } else {
                printf("用法: touch <文件名> <大小>\n");
            }
        } else if (strcmp(arg1, "rm") == 0) {
            if (argc >= 2) {
                delete_file(arg2);
            } else {
                printf("用法: rm <文件名>\n");
            }
        } else if (strcmp(arg1, "cd") == 0) {
            if (argc >= 2) {
                change_directory(arg2);
            } else {
                printf("用法: cd <目录名>\n");
            }
        } else if (strcmp(arg1, "pwd") == 0) {
            printf("\n");
            print_current_path();
            printf("\n");
        } else if (strcmp(arg1, "info") == 0) {
            if (argc >= 2) {
                print_file_info(arg2);
            } else {
                printf("用法: info <文件名>\n");
            }
        } else if (strcmp(arg1, "df") == 0) {
            print_disk_usage();
        } else if (strcmp(arg1, "help") == 0) {
            printf("\n可用命令:\n");
            printf("  ls              - 列出当前目录内容\n");
            printf("  mkdir <name>    - 创建目录\n");
            printf("  touch <name> <size> - 创建文件\n");
            printf("  rm <name>       - 删除文件或目录\n");
            printf("  cd <name>       - 切换目录\n");
            printf("  pwd             - 显示当前路径\n");
            printf("  info <name>     - 显示文件详细信息\n");
            printf("  df              - 显示磁盘使用情况\n");
            printf("  help            - 显示帮助信息\n");
            printf("  exit            - 退出程序\n");
        } else {
            printf("未知命令: %s (输入 'help' 查看帮助)\n", arg1);
        }
    }
    
    return 0;
}