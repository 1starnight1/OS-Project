#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define BUFFER_SIZE 5
#define PRODUCER_COUNT 2
#define CONSUMER_COUNT 2

sem_t empty, full;
pthread_mutex_t mutex;
int buffer[BUFFER_SIZE];
int in = 0, out = 0;

void *producer(void *arg) {
    int producer_id = *((int *)arg);
    
    for (int i = 0; i < 5; i++) {
        int item = producer_id * 10 + i;
        
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        
        buffer[in] = item;
        printf("生产者 P%d 生产了产品 %d (位置 %d)\n", producer_id, item, in);
        in = (in + 1) % BUFFER_SIZE;
        
        pthread_mutex_unlock(&mutex);
        sem_post(&full);
        
        sleep(1);
    }
    
    return NULL;
}

void *consumer(void *arg) {
    int consumer_id = *((int *)arg);
    
    for (int i = 0; i < 5; i++) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        
        int item = buffer[out];
        printf("消费者 C%d 消费了产品 %d (位置 %d)\n", consumer_id, item, out);
        out = (out + 1) % BUFFER_SIZE;
        
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
        
        sleep(2);
    }
    
    return NULL;
}

void producer_consumer() {
    pthread_t producers[PRODUCER_COUNT];
    pthread_t consumers[CONSUMER_COUNT];
    int producer_ids[PRODUCER_COUNT];
    int consumer_ids[CONSUMER_COUNT];
    
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);
    
    printf("========================================\n");
    printf("       生产者-消费者问题模拟\n");
    printf("========================================\n\n");
    
    for (int i = 0; i < PRODUCER_COUNT; i++) {
        producer_ids[i] = i + 1;
        pthread_create(&producers[i], NULL, producer, &producer_ids[i]);
    }
    
    for (int i = 0; i < CONSUMER_COUNT; i++) {
        consumer_ids[i] = i + 1;
        pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]);
    }
    
    for (int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_join(producers[i], NULL);
    }
    
    for (int i = 0; i < CONSUMER_COUNT; i++) {
        pthread_join(consumers[i], NULL);
    }
    
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
    
    printf("\n生产者-消费者模拟完成\n");
}

// 读者-写者问题
int read_count = 0;
sem_t mutex_rw, wrt;

void *reader(void *arg) {
    int reader_id = *((int *)arg);
    
    sem_wait(&mutex_rw);
    read_count++;
    if (read_count == 1) {
        sem_wait(&wrt);
    }
    sem_post(&mutex_rw);
    
    printf("读者 R%d 正在读取数据\n", reader_id);
    sleep(1);
    printf("读者 R%d 读取完成\n", reader_id);
    
    sem_wait(&mutex_rw);
    read_count--;
    if (read_count == 0) {
        sem_post(&wrt);
    }
    sem_post(&mutex_rw);
    
    return NULL;
}

void *writer(void *arg) {
    int writer_id = *((int *)arg);
    
    sem_wait(&wrt);
    
    printf("写者 W%d 正在写入数据\n", writer_id);
    sleep(2);
    printf("写者 W%d 写入完成\n", writer_id);
    
    sem_post(&wrt);
    
    return NULL;
}

void reader_writer() {
    pthread_t readers[5];
    pthread_t writers[3];
    int reader_ids[5];
    int writer_ids[3];
    
    sem_init(&mutex_rw, 0, 1);
    sem_init(&wrt, 0, 1);
    
    printf("\n========================================\n");
    printf("       读者-写者问题模拟\n");
    printf("========================================\n\n");
    
    for (int i = 0; i < 5; i++) {
        reader_ids[i] = i + 1;
        pthread_create(&readers[i], NULL, reader, &reader_ids[i]);
    }
    
    for (int i = 0; i < 3; i++) {
        writer_ids[i] = i + 1;
        pthread_create(&writers[i], NULL, writer, &writer_ids[i]);
    }
    
    for (int i = 0; i < 5; i++) {
        pthread_join(readers[i], NULL);
    }
    
    for (int i = 0; i < 3; i++) {
        pthread_join(writers[i], NULL);
    }
    
    sem_destroy(&mutex_rw);
    sem_destroy(&wrt);
    
    printf("\n读者-写者模拟完成\n");
}

// 哲学家进餐问题
#define PHILOSOPHERS 5
sem_t chopsticks[PHILOSOPHERS];

void *philosopher(void *arg) {
    int philosopher_id = *((int *)arg);
    int left = philosopher_id;
    int right = (philosopher_id + 1) % PHILOSOPHERS;
    
    for (int i = 0; i < 2; i++) {
        printf("哲学家 %d 正在思考\n", philosopher_id);
        sleep(1);
        
        printf("哲学家 %d 饿了，准备进餐\n", philosopher_id);
        
        // 避免死锁：先拿编号小的筷子
        if (left < right) {
            sem_wait(&chopsticks[left]);
            sem_wait(&chopsticks[right]);
        } else {
            sem_wait(&chopsticks[right]);
            sem_wait(&chopsticks[left]);
        }
        
        printf("哲学家 %d 拿到筷子 %d 和 %d，开始进餐\n", 
               philosopher_id, left, right);
        sleep(2);
        
        printf("哲学家 %d 进餐完成，放下筷子\n", philosopher_id);
        
        sem_post(&chopsticks[left]);
        sem_post(&chopsticks[right]);
    }
    
    return NULL;
}

void dining_philosophers() {
    pthread_t philosophers[PHILOSOPHERS];
    int philosopher_ids[PHILOSOPHERS];
    
    for (int i = 0; i < PHILOSOPHERS; i++) {
        sem_init(&chopsticks[i], 0, 1);
    }
    
    printf("\n========================================\n");
    printf("       哲学家进餐问题模拟\n");
    printf("========================================\n\n");
    
    for (int i = 0; i < PHILOSOPHERS; i++) {
        philosopher_ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher, &philosopher_ids[i]);
    }
    
    for (int i = 0; i < PHILOSOPHERS; i++) {
        pthread_join(philosophers[i], NULL);
    }
    
    for (int i = 0; i < PHILOSOPHERS; i++) {
        sem_destroy(&chopsticks[i]);
    }
    
    printf("\n哲学家进餐模拟完成\n");
}

int main() {
    int choice;
    
    printf("========================================\n");
    printf("       进程同步与并发控制模拟系统\n");
    printf("========================================\n\n");
    
    while (1) {
        printf("请选择要模拟的问题:\n");
        printf("1. 生产者-消费者问题\n");
        printf("2. 读者-写者问题\n");
        printf("3. 哲学家进餐问题\n");
        printf("4. 退出\n");
        printf("选择: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                producer_consumer();
                break;
            case 2:
                reader_writer();
                break;
            case 3:
                dining_philosophers();
                break;
            case 4:
                printf("程序结束\n");
                return 0;
            default:
                printf("无效选择，请重新输入\n");
        }
        
        printf("\n");
    }
    
    return 0;
}