#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_QUEUES 3
#define BASE_QUANTUM 2

typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;
    int remaining_time;
    int start_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
    int response_time;
} Process;

typedef struct {
    int time;
    int pid;
    int duration;
} ScheduleEvent;

typedef struct {
    const char *name;
    float avg_turnaround;
    float avg_waiting;
    float avg_response;
    float throughput;
} PerformanceResult;

void fcfs(Process *processes, int n, ScheduleEvent *schedule, int *schedule_count) {
    *schedule_count = 0;
    int current_time = 0;
    
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrival_time > processes[j + 1].arrival_time) {
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
    
    for (int i = 0; i < n; i++) {
        if (current_time < processes[i].arrival_time) {
            current_time = processes[i].arrival_time;
        }
        
        processes[i].start_time = current_time;
        processes[i].completion_time = current_time + processes[i].burst_time;
        processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
        processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
        processes[i].response_time = processes[i].start_time - processes[i].arrival_time;
        
        schedule[*schedule_count].time = current_time;
        schedule[*schedule_count].pid = processes[i].pid;
        schedule[*schedule_count].duration = processes[i].burst_time;
        (*schedule_count)++;
        
        current_time = processes[i].completion_time;
    }
}

void sjf(Process *processes, int n, ScheduleEvent *schedule, int *schedule_count) {
    *schedule_count = 0;
    int current_time = 0;
    int completed = 0;
    int *completed_flag = (int *)calloc(n, sizeof(int));
    
    while (completed < n) {
        int shortest = -1;
        int min_burst = 999999;
        
        for (int i = 0; i < n; i++) {
            if (!completed_flag[i] && processes[i].arrival_time <= current_time) {
                if (processes[i].burst_time < min_burst) {
                    min_burst = processes[i].burst_time;
                    shortest = i;
                }
            }
        }
        
        if (shortest == -1) {
            current_time++;
            continue;
        }
        
        processes[shortest].start_time = current_time;
        processes[shortest].completion_time = current_time + processes[shortest].burst_time;
        processes[shortest].turnaround_time = processes[shortest].completion_time - processes[shortest].arrival_time;
        processes[shortest].waiting_time = processes[shortest].turnaround_time - processes[shortest].burst_time;
        processes[shortest].response_time = processes[shortest].start_time - processes[shortest].arrival_time;
        
        schedule[*schedule_count].time = current_time;
        schedule[*schedule_count].pid = processes[shortest].pid;
        schedule[*schedule_count].duration = processes[shortest].burst_time;
        (*schedule_count)++;
        
        current_time = processes[shortest].completion_time;
        completed_flag[shortest] = 1;
        completed++;
    }
    
    free(completed_flag);
}

void round_robin(Process *processes, int n, int time_quantum, ScheduleEvent *schedule, int *schedule_count) {
    *schedule_count = 0;
    int current_time = 0;
    int completed = 0;
    int *remaining_time = (int *)malloc(n * sizeof(int));
    
    for (int i = 0; i < n; i++) {
        remaining_time[i] = processes[i].burst_time;
        processes[i].response_time = -1;
    }
    
    while (completed < n) {
        int found = 0;
        
        for (int i = 0; i < n; i++) {
            if (remaining_time[i] > 0 && processes[i].arrival_time <= current_time) {
                found = 1;
                
                if (processes[i].response_time == -1) {
                    processes[i].response_time = current_time - processes[i].arrival_time;
                }
                
                int run_time = (remaining_time[i] < time_quantum) ? remaining_time[i] : time_quantum;
                
                schedule[*schedule_count].time = current_time;
                schedule[*schedule_count].pid = processes[i].pid;
                schedule[*schedule_count].duration = run_time;
                (*schedule_count)++;
                
                remaining_time[i] -= run_time;
                current_time += run_time;
                
                if (remaining_time[i] == 0) {
                    processes[i].completion_time = current_time;
                    processes[i].turnaround_time = processes[i].completion_time - processes[i].arrival_time;
                    processes[i].waiting_time = processes[i].turnaround_time - processes[i].burst_time;
                    completed++;
                }
            }
        }
        
        if (!found) {
            current_time++;
        }
    }
    
    free(remaining_time);
}

void priority_non_preemptive(Process *processes, int n, ScheduleEvent *schedule, int *schedule_count) {
    *schedule_count = 0;
    int current_time = 0;
    int completed = 0;
    int *completed_flag = (int *)calloc(n, sizeof(int));
    
    while (completed < n) {
        int highest = -1;
        int min_priority = 999999;
        
        for (int i = 0; i < n; i++) {
            if (!completed_flag[i] && processes[i].arrival_time <= current_time) {
                if (processes[i].priority < min_priority) {
                    min_priority = processes[i].priority;
                    highest = i;
                }
            }
        }
        
        if (highest == -1) {
            current_time++;
            continue;
        }
        
        processes[highest].start_time = current_time;
        processes[highest].completion_time = current_time + processes[highest].burst_time;
        processes[highest].turnaround_time = processes[highest].completion_time - processes[highest].arrival_time;
        processes[highest].waiting_time = processes[highest].turnaround_time - processes[highest].burst_time;
        processes[highest].response_time = processes[highest].start_time - processes[highest].arrival_time;
        
        schedule[*schedule_count].time = current_time;
        schedule[*schedule_count].pid = processes[highest].pid;
        schedule[*schedule_count].duration = processes[highest].burst_time;
        (*schedule_count)++;
        
        current_time = processes[highest].completion_time;
        completed_flag[highest] = 1;
        completed++;
    }
    
    free(completed_flag);
}

void priority_preemptive(Process *processes, int n, ScheduleEvent *schedule, int *schedule_count) {
    *schedule_count = 0;
    int current_time = 0;
    int completed = 0;
    int *completed_flag = (int *)calloc(n, sizeof(int));
    int current_process = -1;
    int start_time = 0;
    
    for (int i = 0; i < n; i++) {
        processes[i].response_time = -1;
        processes[i].remaining_time = processes[i].burst_time;
    }
    
    while (completed < n) {
        int highest = -1;
        int min_priority = 999999;
        
        for (int i = 0; i < n; i++) {
            if (!completed_flag[i] && processes[i].arrival_time <= current_time) {
                if (processes[i].priority < min_priority) {
                    min_priority = processes[i].priority;
                    highest = i;
                }
            }
        }
        
        if (highest == -1) {
            current_time++;
            continue;
        }
        
        if (current_process != highest) {
            if (current_process != -1 && !completed_flag[current_process]) {
                int duration = current_time - start_time;
                if (duration > 0) {
                    schedule[*schedule_count].time = start_time;
                    schedule[*schedule_count].pid = processes[current_process].pid;
                    schedule[*schedule_count].duration = duration;
                    (*schedule_count)++;
                }
            }
            
            current_process = highest;
            start_time = current_time;
            
            if (processes[highest].response_time == -1) {
                processes[highest].response_time = current_time - processes[highest].arrival_time;
            }
        }
        
        processes[current_process].remaining_time--;
        current_time++;
        
        if (processes[current_process].remaining_time == 0) {
            int duration = current_time - start_time;
            schedule[*schedule_count].time = start_time;
            schedule[*schedule_count].pid = processes[current_process].pid;
            schedule[*schedule_count].duration = duration;
            (*schedule_count)++;
            
            processes[current_process].completion_time = current_time;
            processes[current_process].turnaround_time = processes[current_process].completion_time - processes[current_process].arrival_time;
            processes[current_process].waiting_time = processes[current_process].turnaround_time - processes[current_process].burst_time;
            
            completed_flag[current_process] = 1;
            completed++;
            current_process = -1;
        }
    }
    
    free(completed_flag);
}

void mlfq(Process *processes, int n, ScheduleEvent *schedule, int *schedule_count) {
    *schedule_count = 0;
    int current_time = 0;
    int completed = 0;
    int *completed_flag = (int *)calloc(n, sizeof(int));
    int *queue_level = (int *)malloc(n * sizeof(int));
    int *remaining_time = (int *)malloc(n * sizeof(int));
    
    for (int i = 0; i < n; i++) {
        queue_level[i] = 0;
        remaining_time[i] = processes[i].burst_time;
        processes[i].response_time = -1;
    }
    
    while (completed < n) {
        int found = 0;
        int selected = -1;
        
        for (int q = 0; q < NUM_QUEUES; q++) {
            for (int i = 0; i < n; i++) {
                if (!completed_flag[i] && processes[i].arrival_time <= current_time && queue_level[i] == q) {
                    selected = i;
                    found = 1;
                    break;
                }
            }
            if (found) break;
        }
        
        if (!found) {
            current_time++;
            continue;
        }
        
        int quantum = BASE_QUANTUM * (1 << queue_level[selected]);
        int run_time = (remaining_time[selected] < quantum) ? remaining_time[selected] : quantum;
        
        if (processes[selected].response_time == -1) {
            processes[selected].response_time = current_time - processes[selected].arrival_time;
        }
        
        schedule[*schedule_count].time = current_time;
        schedule[*schedule_count].pid = processes[selected].pid;
        schedule[*schedule_count].duration = run_time;
        (*schedule_count)++;
        
        remaining_time[selected] -= run_time;
        current_time += run_time;
        
        if (remaining_time[selected] == 0) {
            processes[selected].completion_time = current_time;
            processes[selected].turnaround_time = processes[selected].completion_time - processes[selected].arrival_time;
            processes[selected].waiting_time = processes[selected].turnaround_time - processes[selected].burst_time;
            completed_flag[selected] = 1;
            completed++;
        } else {
            if (queue_level[selected] < NUM_QUEUES - 1) {
                queue_level[selected]++;
            }
        }
    }
    
    free(completed_flag);
    free(queue_level);
    free(remaining_time);
}

void print_gantt(ScheduleEvent *schedule, int schedule_count, int n) {
    int max_time = 0;
    for (int i = 0; i < schedule_count; i++) {
        if (schedule[i].time + schedule[i].duration > max_time) {
            max_time = schedule[i].time + schedule[i].duration;
        }
    }
    
    printf("\n甘特图:\n");
    printf("----------------------------------------\n");
    
    int colors[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    char *color_codes[] = {"\033[41m", "\033[42m", "\033[43m", "\033[44m", "\033[45m", 
                           "\033[46m", "\033[47m", "\033[101m", "\033[102m", "\033[103m"};
    char *reset = "\033[0m";
    
    for (int i = 0; i < schedule_count; i++) {
        int start = schedule[i].time;
        int duration = schedule[i].duration;
        int pid = schedule[i].pid;
        
        printf("%s", color_codes[pid % 10]);
        for (int j = 0; j < duration; j++) {
            if (j == duration / 2) {
                printf("P%d", pid);
                j += 1;
            } else {
                printf(" ");
            }
        }
        printf("%s", reset);
    }
    printf("\n");
    
    for (int i = 0; i <= max_time; i += 5) {
        printf("%-5d", i);
    }
    printf("\n");
    printf("----------------------------------------\n");
}

void print_results(Process *processes, int n, ScheduleEvent *schedule, int schedule_count, const char *algorithm_name) {
    printf("\n========================================\n");
    printf("          %s 调度结果\n", algorithm_name);
    printf("========================================\n\n");
    
    printf("进程执行详情:\n");
    printf("----------------------------------------\n");
    printf("PID\t到达\t执行\t开始\t完成\t周转\t等待\t响应\n");
    printf("----------------------------------------\n");
    
    float total_turnaround = 0, total_waiting = 0, total_response = 0;
    int max_completion = 0;
    
    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
               processes[i].pid,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].start_time,
               processes[i].completion_time,
               processes[i].turnaround_time,
               processes[i].waiting_time,
               processes[i].response_time);
        
        total_turnaround += processes[i].turnaround_time;
        total_waiting += processes[i].waiting_time;
        total_response += processes[i].response_time;
        
        if (processes[i].completion_time > max_completion) {
            max_completion = processes[i].completion_time;
        }
    }
    
    printf("----------------------------------------\n");
    printf("平均周转时间: %.2f\n", total_turnaround / n);
    printf("平均等待时间: %.2f\n", total_waiting / n);
    printf("平均响应时间: %.2f\n", total_response / n);
    printf("吞吐量: %.4f 进程/时间单位\n", (float)n / max_completion);
    
    print_gantt(schedule, schedule_count, n);
}

void run_all_algorithms(Process *original, int n) {
    ScheduleEvent *schedule = (ScheduleEvent *)malloc(1000 * sizeof(ScheduleEvent));
    int schedule_count;
    PerformanceResult results[6];
    int result_count = 0;
    
    Process *processes = (Process *)malloc(n * sizeof(Process));
    
    printf("正在执行 FCFS 调度算法...\n");
    fflush(stdout);
    memcpy(processes, original, n * sizeof(Process));
    fcfs(processes, n, schedule, &schedule_count);
    print_results(processes, n, schedule, schedule_count, "FCFS");
    float fcfs_tat = 0, fcfs_wt = 0, fcfs_rt = 0;
    int fcfs_max = 0;
    for (int i = 0; i < n; i++) {
        fcfs_tat += processes[i].turnaround_time;
        fcfs_wt += processes[i].waiting_time;
        fcfs_rt += processes[i].response_time;
        if (processes[i].completion_time > fcfs_max) fcfs_max = processes[i].completion_time;
    }
    results[result_count++] = (PerformanceResult){"FCFS", fcfs_tat/n, fcfs_wt/n, fcfs_rt/n, (float)n/fcfs_max};
    
    printf("\n正在执行 SJF 调度算法...\n");
    fflush(stdout);
    memcpy(processes, original, n * sizeof(Process));
    sjf(processes, n, schedule, &schedule_count);
    print_results(processes, n, schedule, schedule_count, "SJF");
    float sjf_tat = 0, sjf_wt = 0, sjf_rt = 0;
    int sjf_max = 0;
    for (int i = 0; i < n; i++) {
        sjf_tat += processes[i].turnaround_time;
        sjf_wt += processes[i].waiting_time;
        sjf_rt += processes[i].response_time;
        if (processes[i].completion_time > sjf_max) sjf_max = processes[i].completion_time;
    }
    results[result_count++] = (PerformanceResult){"SJF", sjf_tat/n, sjf_wt/n, sjf_rt/n, (float)n/sjf_max};
    
    printf("\n正在执行 RR 调度算法...\n");
    fflush(stdout);
    memcpy(processes, original, n * sizeof(Process));
    round_robin(processes, n, 2, schedule, &schedule_count);
    print_results(processes, n, schedule, schedule_count, "Round Robin");
    float rr_tat = 0, rr_wt = 0, rr_rt = 0;
    int rr_max = 0;
    for (int i = 0; i < n; i++) {
        rr_tat += processes[i].turnaround_time;
        rr_wt += processes[i].waiting_time;
        rr_rt += processes[i].response_time;
        if (processes[i].completion_time > rr_max) rr_max = processes[i].completion_time;
    }
    results[result_count++] = (PerformanceResult){"RR", rr_tat/n, rr_wt/n, rr_rt/n, (float)n/rr_max};
    
    printf("\n正在执行 Priority(NP) 调度算法...\n");
    fflush(stdout);
    memcpy(processes, original, n * sizeof(Process));
    priority_non_preemptive(processes, n, schedule, &schedule_count);
    print_results(processes, n, schedule, schedule_count, "Priority (Non-Preemptive)");
    float pnp_tat = 0, pnp_wt = 0, pnp_rt = 0;
    int pnp_max = 0;
    for (int i = 0; i < n; i++) {
        pnp_tat += processes[i].turnaround_time;
        pnp_wt += processes[i].waiting_time;
        pnp_rt += processes[i].response_time;
        if (processes[i].completion_time > pnp_max) pnp_max = processes[i].completion_time;
    }
    results[result_count++] = (PerformanceResult){"Priority(NP)", pnp_tat/n, pnp_wt/n, pnp_rt/n, (float)n/pnp_max};
    
    printf("\n正在执行 Priority(P) 调度算法...\n");
    fflush(stdout);
    memcpy(processes, original, n * sizeof(Process));
    priority_preemptive(processes, n, schedule, &schedule_count);
    print_results(processes, n, schedule, schedule_count, "Priority (Preemptive)");
    float pp_tat = 0, pp_wt = 0, pp_rt = 0;
    int pp_max = 0;
    for (int i = 0; i < n; i++) {
        pp_tat += processes[i].turnaround_time;
        pp_wt += processes[i].waiting_time;
        pp_rt += processes[i].response_time;
        if (processes[i].completion_time > pp_max) pp_max = processes[i].completion_time;
    }
    results[result_count++] = (PerformanceResult){"Priority(P)", pp_tat/n, pp_wt/n, pp_rt/n, (float)n/pp_max};
    
    printf("\n正在执行 MLFQ 调度算法...\n");
    fflush(stdout);
    memcpy(processes, original, n * sizeof(Process));
    mlfq(processes, n, schedule, &schedule_count);
    print_results(processes, n, schedule, schedule_count, "Multi-Level Feedback Queue");
    float mlfq_tat = 0, mlfq_wt = 0, mlfq_rt = 0;
    int mlfq_max = 0;
    for (int i = 0; i < n; i++) {
        mlfq_tat += processes[i].turnaround_time;
        mlfq_wt += processes[i].waiting_time;
        mlfq_rt += processes[i].response_time;
        if (processes[i].completion_time > mlfq_max) mlfq_max = processes[i].completion_time;
    }
    results[result_count++] = (PerformanceResult){"MLFQ", mlfq_tat/n, mlfq_wt/n, mlfq_rt/n, (float)n/mlfq_max};
    
    printf("\n\n========================================\n");
    printf("          调度算法性能对比\n");
    printf("========================================\n");
    printf("%-15s\t%-12s\t%-12s\t%-12s\t%-12s\n", 
           "算法", "平均周转时间", "平均等待时间", "平均响应时间", "吞吐量");
    printf("-------------------------------------------------------------------------\n");
    
    for (int i = 0; i < result_count; i++) {
        printf("%-15s\t%.2f\t\t%.2f\t\t%.2f\t\t%.4f\n",
               results[i].name,
               results[i].avg_turnaround,
               results[i].avg_waiting,
               results[i].avg_response,
               results[i].throughput);
    }
    
    printf("-------------------------------------------------------------------------\n");
    
    int best_tat = 0, best_wt = 0, best_rt = 0, best_tp = 0;
    for (int i = 1; i < result_count; i++) {
        if (results[i].avg_turnaround < results[best_tat].avg_turnaround) best_tat = i;
        if (results[i].avg_waiting < results[best_wt].avg_waiting) best_wt = i;
        if (results[i].avg_response < results[best_rt].avg_response) best_rt = i;
        if (results[i].throughput > results[best_tp].throughput) best_tp = i;
    }
    
    printf("\n性能最优分析:\n");
    printf("✓ 平均周转时间最优: %s (%.2f)\n", results[best_tat].name, results[best_tat].avg_turnaround);
    printf("✓ 平均等待时间最优: %s (%.2f)\n", results[best_wt].name, results[best_wt].avg_waiting);
    printf("✓ 平均响应时间最优: %s (%.2f)\n", results[best_rt].name, results[best_rt].avg_response);
    printf("✓ 吞吐量最优: %s (%.4f)\n", results[best_tp].name, results[best_tp].throughput);
    
    free(processes);
    free(schedule);
}

void benchmark_test() {
    printf("\n========================================\n");
    printf("          调度算法性能基准测试\n");
    printf("========================================\n\n");
    
    int test_cases[4][5][3] = {
        {{0, 5, 2}, {1, 3, 1}, {2, 1, 3}, {4, 6, 2}, {5, 2, 1}},
        {{0, 10, 1}, {2, 3, 2}, {4, 5, 3}, {6, 2, 4}, {8, 4, 5}},
        {{0, 8, 1}, {1, 4, 2}, {2, 2, 3}, {3, 6, 4}, {4, 3, 5}},
        {{0, 15, 1}, {3, 5, 2}, {5, 3, 3}, {7, 10, 4}, {10, 2, 5}}
    };
    
    for (int t = 0; t < 4; t++) {
        printf("测试用例 %d:\n", t + 1);
        printf("进程: ");
        for (int i = 0; i < 5; i++) {
            printf("P%d(%d,%d) ", i + 1, test_cases[t][i][0], test_cases[t][i][1]);
        }
        printf("\n");
        
        Process *original = (Process *)malloc(5 * sizeof(Process));
        for (int i = 0; i < 5; i++) {
            original[i].pid = i + 1;
            original[i].arrival_time = test_cases[t][i][0];
            original[i].burst_time = test_cases[t][i][1];
            original[i].priority = test_cases[t][i][2];
            original[i].start_time = -1;
        }
        
        run_all_algorithms(original, 5);
        free(original);
        
        printf("\n");
    }
}

int main() {
    printf("========================================\n");
    printf("    操作系统处理机调度算法仿真系统\n");
    printf("          版本 2.0 - 性能优化版\n");
    printf("========================================\n\n");
    
    printf("请选择运行模式:\n");
    printf("1. 手动输入进程参数\n");
    printf("2. 运行性能基准测试\n");
    printf("选择: ");
    
    int mode;
    scanf("%d", &mode);
    
    if (mode == 2) {
        benchmark_test();
        return 0;
    }
    
    int n;
    printf("\n请输入进程数量: ");
    scanf("%d", &n);
    
    Process *processes = (Process *)malloc(n * sizeof(Process));
    ScheduleEvent *schedule = (ScheduleEvent *)malloc(1000 * sizeof(ScheduleEvent));
    int schedule_count;
    
    printf("\n请输入进程信息 (到达时间 执行时间 优先级):\n");
    for (int i = 0; i < n; i++) {
        processes[i].pid = i + 1;
        processes[i].start_time = -1;
        processes[i].remaining_time = 0;
        printf("P%d: ", i + 1);
        scanf("%d %d %d", &processes[i].arrival_time, &processes[i].burst_time, &processes[i].priority);
    }
    
    Process *original = (Process *)malloc(n * sizeof(Process));
    memcpy(original, processes, n * sizeof(Process));
    
    printf("\n请选择要执行的调度算法:\n");
    printf("1. FCFS (先来先服务)\n");
    printf("2. SJF (短作业优先)\n");
    printf("3. RR (时间片轮转)\n");
    printf("4. Priority (非抢占式)\n");
    printf("5. Priority (抢占式)\n");
    printf("6. MLFQ (多级反馈队列)\n");
    printf("7. 运行所有算法并对比\n");
    printf("选择: ");
    
    int choice;
    scanf("%d", &choice);
    
    switch (choice) {
        case 1:
            memcpy(processes, original, n * sizeof(Process));
            fcfs(processes, n, schedule, &schedule_count);
            print_results(processes, n, schedule, schedule_count, "FCFS");
            break;
            
        case 2:
            memcpy(processes, original, n * sizeof(Process));
            sjf(processes, n, schedule, &schedule_count);
            print_results(processes, n, schedule, schedule_count, "SJF");
            break;
            
        case 3: {
            int quantum;
            printf("请输入时间片大小: ");
            scanf("%d", &quantum);
            memcpy(processes, original, n * sizeof(Process));
            round_robin(processes, n, quantum, schedule, &schedule_count);
            print_results(processes, n, schedule, schedule_count, "Round Robin");
            break;
        }
            
        case 4:
            memcpy(processes, original, n * sizeof(Process));
            priority_non_preemptive(processes, n, schedule, &schedule_count);
            print_results(processes, n, schedule, schedule_count, "Priority (Non-Preemptive)");
            break;
            
        case 5:
            memcpy(processes, original, n * sizeof(Process));
            priority_preemptive(processes, n, schedule, &schedule_count);
            print_results(processes, n, schedule, schedule_count, "Priority (Preemptive)");
            break;
            
        case 6:
            memcpy(processes, original, n * sizeof(Process));
            mlfq(processes, n, schedule, &schedule_count);
            print_results(processes, n, schedule, schedule_count, "Multi-Level Feedback Queue");
            break;
            
        case 7:
            run_all_algorithms(original, n);
            break;
            
        default:
            printf("无效选择\n");
    }
    
    free(processes);
    free(schedule);
    free(original);
    
    return 0;
}