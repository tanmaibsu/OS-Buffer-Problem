/**Update this file with the starter code**/
#include<pthread.h>
#include<stdbool.h>
#include<stdlib.h>
#include<stdio.h>
#include "../src/lab.h"

struct queue {
    void **data;
    int front;
    int back;
    int len;
    int capacity;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    bool shut;
};

queue_t queue_init(int capacity) {
    queue_t que = (queue_t)malloc(sizeof(struct queue));

    if(!que) return NULL;

    que->data = (void **)malloc(sizeof(void *) * capacity);
    if(!que->data) {
        free(que);
        return NULL;
    }
    que->capacity = capacity;
    que->front = 0;
    que->back = -1;
    que->len = 0;
    que->shut = false;
    pthread_mutex_init(&que->lock, NULL);
    pthread_cond_init(&que->cond, NULL);
    return que;
}

void queue_destroy(queue_t que) {
    if(que) {
        pthread_mutex_lock(&que->lock);
        que->shut = true;
        pthread_cond_broadcast(&que->cond);
        pthread_mutex_unlock(&que->lock);
        free(que->data);
        pthread_mutex_destroy(&que->lock);
        pthread_cond_destroy(&que->cond);
        free(que);
    }
}

void enqueue(queue_t que, void *data) {
    pthread_mutex_lock(&que->lock);

    while(que->len == que->capacity && !que->shut) {
        pthread_cond_wait(&que->cond, &que->lock);

        if(que->shut) {
            pthread_mutex_unlock(&que->lock);
            return;
        }
        que->back = (que->back + 1) % que->capacity;
        que->data[que->back] = data;
        que->len++;
        pthread_cond_signal(&que->cond);
        pthread_mutex_unlock(&que->lock);
    }
}

void *dequeue(queue_t que) {
    pthread_mutex_lock(&que->lock);
    printf("I am here.\n");
    while(que->len == 0 && !que->shut) {
        printf("I am here.\n");
        pthread_cond_wait(&que->cond, &que->lock);
    }
    if(que->shut && que->len == 0) {
        pthread_mutex_unlock(&que->lock);
        return NULL;
    }

    void *data = que->data[que->front];
    que->front = (que->front + 1) % que->capacity;
    que->len--;
    pthread_cond_signal(&que->cond);
    pthread_mutex_unlock(&que->lock);
    return data;
}

void queue_shutdown(queue_t que) {
    pthread_mutex_lock(&que->lock);
    que->shut = true;
    pthread_cond_broadcast(&que->cond);
    pthread_mutex_unlock(&que->lock);
    return;
}

bool is_empty(queue_t que) {
    pthread_mutex_lock(&que->lock);
    bool empty = (que->len == 0);
    pthread_mutex_unlock(&que->lock);
    return empty;
}

bool is_shutdown(queue_t que) {
    pthread_mutex_lock(&que->lock);
    bool shut = que->shut;
    pthread_mutex_unlock(&que->lock);
    return shut;
}

