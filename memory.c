#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MEMORY 1024
#define MAX_PARTITIONS 100

typedef struct {
    int start;
    int size;
    int is_free;
    int process_id;
} Partition;

typedef struct {
    int page_number;
    int frame_number;
    int is_valid;
} PageTableEntry;

Partition partitions[MAX_PARTITIONS];
int partition_count = 0;

void init_memory() {
    partitions[0].start = 0;
    partitions[0].size = MAX_MEMORY;
    partitions[0].is_free = 1;
    partitions[0].process_id = -1;
    partition_count = 1;
}

void print_partitions() {
    printf("\n内存分区状态:\n");
    printf("----------------------------------------\n");
    printf("起始地址\t大小\t状态\t进程ID\n");
    printf("----------------------------------------\n");
    
    for (int i = 0; i < partition_count; i++) {
        printf("%d\t\t%d\t%s\t", partitions[i].start, partitions[i].size,
               partitions[i].is_free ? "空闲" : "占用");
        if (partitions[i].is_free) {
            printf("-\n");
        } else {
            printf("P%d\n", partitions[i].process_id);
        }
    }
    printf("----------------------------------------\n");
}

void first_fit(int process_id, int size) {
    for (int i = 0; i < partition_count; i++) {
        if (partitions[i].is_free && partitions[i].size >= size) {
            if (partitions[i].size > size) {
                // 分割分区
                for (int j = partition_count; j > i + 1; j--) {
                    partitions[j] = partitions[j - 1];
                }
                
                partitions[i + 1].start = partitions[i].start + size;
                partitions[i + 1].size = partitions[i].size - size;
                partitions[i + 1].is_free = 1;
                partitions[i + 1].process_id = -1;
                
                partitions[i].size = size;
                partition_count++;
            }
            
            partitions[i].is_free = 0;
            partitions[i].process_id = process_id;
            
            printf("进程 P%d 成功分配内存: %d 字节 (地址 %d-%d)\n",
                   process_id, size, partitions[i].start, partitions[i].start + size - 1);
            return;
        }
    }
    
    printf("进程 P%d 内存分配失败: 没有足够的连续空间\n", process_id);
}

void best_fit(int process_id, int size) {
    int best_index = -1;
    int min_diff = MAX_MEMORY;
    
    for (int i = 0; i < partition_count; i++) {
        if (partitions[i].is_free && partitions[i].size >= size) {
            int diff = partitions[i].size - size;
            if (diff < min_diff) {
                min_diff = diff;
                best_index = i;
            }
        }
    }
    
    if (best_index != -1) {
        if (partitions[best_index].size > size) {
            for (int j = partition_count; j > best_index + 1; j--) {
                partitions[j] = partitions[j - 1];
            }
            
            partitions[best_index + 1].start = partitions[best_index].start + size;
            partitions[best_index + 1].size = partitions[best_index].size - size;
            partitions[best_index + 1].is_free = 1;
            partitions[best_index + 1].process_id = -1;
            
            partitions[best_index].size = size;
            partition_count++;
        }
        
        partitions[best_index].is_free = 0;
        partitions[best_index].process_id = process_id;
        
        printf("进程 P%d 成功分配内存: %d 字节 (地址 %d-%d)\n",
               process_id, size, partitions[best_index].start, 
               partitions[best_index].start + size - 1);
    } else {
        printf("进程 P%d 内存分配失败: 没有足够的连续空间\n", process_id);
    }
}

void deallocate(int process_id) {
    for (int i = 0; i < partition_count; i++) {
        if (!partitions[i].is_free && partitions[i].process_id == process_id) {
            partitions[i].is_free = 1;
            partitions[i].process_id = -1;
            
            // 合并相邻的空闲分区
            if (i > 0 && partitions[i - 1].is_free) {
                partitions[i - 1].size += partitions[i].size;
                for (int j = i; j < partition_count - 1; j++) {
                    partitions[j] = partitions[j + 1];
                }
                partition_count--;
                i--;
            }
            
            if (i < partition_count - 1 && partitions[i + 1].is_free) {
                partitions[i].size += partitions[i + 1].size;
                for (int j = i + 1; j < partition_count - 1; j++) {
                    partitions[j] = partitions[j + 1];
                }
                partition_count--;
            }
            
            printf("进程 P%d 内存释放成功\n", process_id);
            return;
        }
    }
    
    printf("进程 P%d 内存释放失败: 未找到该进程的内存块\n", process_id);
}

// 页面置换算法
int fifo(int *pages, int page_count, int frame_count) {
    int *frames = (int *)malloc(frame_count * sizeof(int));
    int *page_queue = (int *)malloc(page_count * sizeof(int));
    int page_faults = 0;
    int queue_front = 0, queue_rear = 0;
    
    for (int i = 0; i < frame_count; i++) {
        frames[i] = -1;
    }
    
    printf("\nFIFO 页面置换过程:\n");
    printf("----------------------------------------\n");
    
    for (int i = 0; i < page_count; i++) {
        int page = pages[i];
        int found = 0;
        
        for (int j = 0; j < frame_count; j++) {
            if (frames[j] == page) {
                found = 1;
                break;
            }
        }
        
        if (!found) {
            page_faults++;
            
            if (queue_rear - queue_front < frame_count) {
                frames[queue_rear % frame_count] = page;
                page_queue[queue_rear] = page;
                queue_rear++;
            } else {
                int replace_page = page_queue[queue_front % frame_count];
                for (int j = 0; j < frame_count; j++) {
                    if (frames[j] == replace_page) {
                        frames[j] = page;
                        break;
                    }
                }
                page_queue[queue_rear] = page;
                queue_front++;
                queue_rear++;
            }
            
            printf("页面 %d: 缺页 - 框架: ", page);
            for (int j = 0; j < frame_count; j++) {
                if (frames[j] != -1) {
                    printf("%d ", frames[j]);
                } else {
                    printf("- ");
                }
            }
            printf("(替换)\n");
        } else {
            printf("页面 %d: 命中\n", page);
        }
    }
    
    printf("----------------------------------------\n");
    printf("总缺页次数: %d\n", page_faults);
    printf("缺页率: %.2f%%\n", (float)page_faults / page_count * 100);
    
    free(frames);
    free(page_queue);
    
    return page_faults;
}

int lru(int *pages, int page_count, int frame_count) {
    int *frames = (int *)malloc(frame_count * sizeof(int));
    int *last_used = (int *)malloc(frame_count * sizeof(int));
    int page_faults = 0;
    int time = 0;
    
    for (int i = 0; i < frame_count; i++) {
        frames[i] = -1;
        last_used[i] = -1;
    }
    
    printf("\nLRU 页面置换过程:\n");
    printf("----------------------------------------\n");
    
    for (int i = 0; i < page_count; i++) {
        int page = pages[i];
        int found = 0;
        int found_index = -1;
        
        for (int j = 0; j < frame_count; j++) {
            if (frames[j] == page) {
                found = 1;
                found_index = j;
                break;
            }
        }
        
        if (!found) {
            page_faults++;
            
            int lru_index = 0;
            for (int j = 1; j < frame_count; j++) {
                if (last_used[j] < last_used[lru_index]) {
                    lru_index = j;
                }
            }
            
            frames[lru_index] = page;
            last_used[lru_index] = time;
            
            printf("页面 %d: 缺页 - 框架: ", page);
            for (int j = 0; j < frame_count; j++) {
                if (frames[j] != -1) {
                    printf("%d ", frames[j]);
                } else {
                    printf("- ");
                }
            }
            printf("(替换)\n");
        } else {
            last_used[found_index] = time;
            printf("页面 %d: 命中\n", page);
        }
        
        time++;
    }
    
    printf("----------------------------------------\n");
    printf("总缺页次数: %d\n", page_faults);
    printf("缺页率: %.2f%%\n", (float)page_faults / page_count * 100);
    
    free(frames);
    free(last_used);
    
    return page_faults;
}

int main() {
    int choice;
    
    printf("========================================\n");
    printf("       内存管理与页面置换模拟系统\n");
    printf("========================================\n\n");
    
    while (1) {
        printf("请选择功能:\n");
        printf("1. 首次适应算法 (First Fit)\n");
        printf("2. 最佳适应算法 (Best Fit)\n");
        printf("3. FIFO 页面置换\n");
        printf("4. LRU 页面置换\n");
        printf("5. 退出\n");
        printf("选择: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1: {
                init_memory();
                print_partitions();
                
                int pid, size;
                printf("\n输入进程ID和内存大小 (输入 -1 结束):\n");
                while (1) {
                    scanf("%d", &pid);
                    if (pid == -1) break;
                    scanf("%d", &size);
                    first_fit(pid, size);
                    print_partitions();
                }
                
                printf("\n输入要释放的进程ID (输入 -1 结束):\n");
                while (1) {
                    scanf("%d", &pid);
                    if (pid == -1) break;
                    deallocate(pid);
                    print_partitions();
                }
                break;
            }
            
            case 2: {
                init_memory();
                print_partitions();
                
                int pid, size;
                printf("\n输入进程ID和内存大小 (输入 -1 结束):\n");
                while (1) {
                    scanf("%d", &pid);
                    if (pid == -1) break;
                    scanf("%d", &size);
                    best_fit(pid, size);
                    print_partitions();
                }
                
                printf("\n输入要释放的进程ID (输入 -1 结束):\n");
                while (1) {
                    scanf("%d", &pid);
                    if (pid == -1) break;
                    deallocate(pid);
                    print_partitions();
                }
                break;
            }
            
            case 3: {
                int page_count, frame_count;
                printf("输入页面数量: ");
                scanf("%d", &page_count);
                printf("输入物理块数量: ");
                scanf("%d", &frame_count);
                
                int *pages = (int *)malloc(page_count * sizeof(int));
                printf("输入页面访问序列: ");
                for (int i = 0; i < page_count; i++) {
                    scanf("%d", &pages[i]);
                }
                
                fifo(pages, page_count, frame_count);
                free(pages);
                break;
            }
            
            case 4: {
                int page_count, frame_count;
                printf("输入页面数量: ");
                scanf("%d", &page_count);
                printf("输入物理块数量: ");
                scanf("%d", &frame_count);
                
                int *pages = (int *)malloc(page_count * sizeof(int));
                printf("输入页面访问序列: ");
                for (int i = 0; i < page_count; i++) {
                    scanf("%d", &pages[i]);
                }
                
                lru(pages, page_count, frame_count);
                free(pages);
                break;
            }
            
            case 5:
                printf("程序结束\n");
                return 0;
                
            default:
                printf("无效选择，请重新输入\n");
        }
    }
    
    return 0;
}