#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

// Shared resources and synchronization primitives
int *buffer;
int buf_size = 0, total_items = 0, prod_count = 0, cons_count = 0;
int in = 0, out = 0, current_item_id = 1;
long long sum_consumed = 0;
int produced_total = 0, consumed_total = 0;

sem_t sem_empty, sem_full; // Semaphores for buffer flow control
pthread_mutex_t mutex, prod_mutex, sum_mutex; // Mutexes for thread safety

// Producer function: Assigns IDs and places items in buffer
void* producer(void* arg) {
    while (1) {
        int item;
        pthread_mutex_lock(&prod_mutex); // Protect the sequence generator
        if (current_item_id > total_items) {
            pthread_mutex_unlock(&prod_mutex);
            break;
        }
        item = current_item_id++;
        produced_total++;
        pthread_mutex_unlock(&prod_mutex);

        sem_wait(&sem_empty); // Wait for an empty slot
        pthread_mutex_lock(&mutex); // Lock for buffer access
        buffer[in] = item;
        in = (in + 1) % buf_size;
        pthread_mutex_unlock(&mutex);
        sem_post(&sem_full); // Notify consumers that data is available
    }
    return NULL;
}

// Consumer function: Retrieves items and calculates sum
void* consumer(void* arg) {
    while (1) {
        int item;
        sem_wait(&sem_full); // Wait for a full slot
        pthread_mutex_lock(&mutex);
        
        item = buffer[out];
        // Sentinel check: if item is -1, termination has started
        if (item == -1) {
            pthread_mutex_unlock(&mutex);
            sem_post(&sem_full); // Pass the sentinel to the next consumer
            break;
        }
        
        buffer[out] = 0; 
        out = (out + 1) % buf_size;
        consumed_total++;
        
        pthread_mutex_lock(&sum_mutex); // Protect global sum update
        sum_consumed += item;
        pthread_mutex_unlock(&sum_mutex);
        
        pthread_mutex_unlock(&mutex);
        sem_post(&sem_empty); // Notify producers that space is free
        
        // If all items are consumed, place a sentinel to stop all consumer threads
        if (consumed_total == total_items) {
            sem_wait(&sem_empty);
            pthread_mutex_lock(&mutex);
            buffer[in] = -1; // Sentinel value
            in = (in + 1) % buf_size;
            pthread_mutex_unlock(&mutex);
            sem_post(&sem_full);
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    // 1. Argument Parsing
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--buf") == 0) buf_size = atoi(argv[++i]);
        else if (strcmp(argv[i], "--producers") == 0) prod_count = atoi(argv[++i]);
        else if (strcmp(argv[i], "--consumers") == 0) cons_count = atoi(argv[++i]);
        else if (strcmp(argv[i], "--items") == 0) total_items = atoi(argv[++i]);
    }

    // 2. Exact Error Reporting as per Behavior Specification
    if (buf_size < 1 || buf_size > 1024) {
        fprintf(stderr, "ERROR: E_RANGE: buf must be in 1..1024\n");
        return 1;
    }
    if (prod_count < 1 || prod_count > 16) {
        fprintf(stderr, "ERROR: E_RANGE: producers must be in 1..16\n");
        return 1;
    }
    if (cons_count < 1 || cons_count > 16) {
        fprintf(stderr, "ERROR: E_RANGE: consumers must be in 1..16\n");
        return 1;
    }
    if (total_items < 0 || total_items > 100000) {
        fprintf(stderr, "ERROR: E_RANGE: items must be in 0..100000\n");
        return 1;
    }

    // 3. Resource Initialization
    buffer = malloc(buf_size * sizeof(int));
    sem_init(&sem_empty, 0, buf_size);
    sem_init(&sem_full, 0, 0);
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&prod_mutex, NULL);
    pthread_mutex_init(&sum_mutex, NULL);

    pthread_t prods[prod_count], cons[cons_count];
    
    // 4. Thread Creation
    for (int i = 0; i < prod_count; i++) pthread_create(&prods[i], NULL, producer, NULL);
    for (int i = 0; i < cons_count; i++) pthread_create(&cons[i], NULL, consumer, NULL);

    // 5. Cleanup
    for (int i = 0; i < prod_count; i++) pthread_join(prods[i], NULL);
    for (int i = 0; i < cons_count; i++) pthread_join(cons[i], NULL);

    // 6. Final Deterministic Summary Output
    printf("OK: PRODUCED %d\n", produced_total);
    printf("OK: CONSUMED %d\n", consumed_total);
    printf("OK: SUM %lld\n", sum_consumed);

    // Check internal invariant before exit
    if (produced_total != consumed_total) {
        fprintf(stderr, "ERROR: E_INVARIANT: produced/consumed mismatch\n");
        return 1;
    }

    free(buffer);
    return 0;
}
