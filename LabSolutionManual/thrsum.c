#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// Structure to pass data to each thread
typedef struct {
    int start;
    int end;
} ThreadData;

long long global_sum = 0;       // Shared variable for the final sum
pthread_mutex_t sum_mutex;      // Mutex to protect global_sum

// Thread function: Calculates sum of a specific range
void* calculate_sum(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    long long local_sum = 0;

    // Calculate sum for the assigned partition
    for (int i = data->start; i <= data->end; i++) {
        local_sum += i;
    }

    // Protect the global update using a Mutex lock
    pthread_mutex_lock(&sum_mutex);
    global_sum += local_sum;
    pthread_mutex_unlock(&sum_mutex);

    return NULL;
}

int main(int argc, char *argv[]) {
    int t = 0;
    long long n = 0;

    // 1. Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--threads") == 0) t = atoi(argv[++i]);
        else if (strcmp(argv[i], "-n") == 0) n = atoll(argv[++i]);
    }

    // Validation: Threads must be in 1..32 and N in 1..1,000,000
    if (t < 1 || t > 32) {
        fprintf(stderr, "ERROR: E_RANGE: threads must be in 1..32\n");
        return 1;
    }
    if (n < 1 || n > 1000000) {
        fprintf(stderr, "ERROR: E_RANGE\n");
        return 1;
    }

    // Initialize the mutex
    pthread_mutex_init(&sum_mutex, NULL);

    pthread_t threads[t];
    ThreadData thread_info[t];
    int base_range = n / t;
    int remainder = n % t;
    int current_start = 1;

    // 2. Partition work and create threads
    for (int i = 0; i < t; i++) {
        thread_info[i].start = current_start;
        thread_info[i].end = current_start + base_range - 1;
        
        // Handle remainder to ensure every number is included exactly once
        if (i < remainder) {
            thread_info[i].end++;
        }
        
        pthread_create(&threads[i], NULL, calculate_sum, &thread_info[i]);
        current_start = thread_info[i].end + 1;
    }

    // 3. Join threads to wait for completion
    for (int i = 0; i < t; i++) {
        pthread_join(threads[i], NULL);
    }

    // Output final deterministic sum
    printf("OK: SUM %lld\n", global_sum);

    // Cleanup mutex
    pthread_mutex_destroy(&sum_mutex);

    return 0;
}
