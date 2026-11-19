#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5
#define ITEMS 5

int buf[N];
int front = 0;
int rear = 0;

sem_t empty;
sem_t full;
sem_t mutexP;
sem_t mutexC;

void* producer(void* arg) {
    int id = *(int*)arg;
    for (int i = 1; i <= ITEMS; i++) {
        int data = id * 100 + i;
        sem_wait(&empty);
        sem_wait(&mutexP);

        buf[rear] = data;
        printf("Productor %d produjo %d en posicion %d\n", id, data, rear);
        rear = (rear + 1) % N;

        sem_post(&mutexP);
        sem_post(&full);
        usleep(100000);
    }
    return NULL;
}

void* consumer(void* arg) {
    int id = *(int*)arg;
    for (int i = 1; i <= ITEMS; i++) {
        sem_wait(&full);
        sem_wait(&mutexC);

        int result = buf[front];
        printf("Consumidor %d consumio %d de posicion %d\n", id, result, front);
        front = (front + 1) % N;

        sem_post(&mutexC);
        sem_post(&empty);
        usleep(150000);
    }
    return NULL;
}

int main() {
    pthread_t prod[2], cons[2];
    int ids[2] = {1, 2};

    sem_init(&empty, 0, N);
    sem_init(&full, 0, 0);
    sem_init(&mutexP, 0, 1);
    sem_init(&mutexC, 0, 1);

    for (int i = 0; i < 2; i++) {
        pthread_create(&prod[i], NULL, producer, &ids[i]);
        pthread_create(&cons[i], NULL, consumer, &ids[i]);
    }

    for (int i = 0; i < 2; i++) {
        pthread_join(prod[i], NULL);
        pthread_join(cons[i], NULL);
    }

    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutexP);
    sem_destroy(&mutexC);

    return 0;
}