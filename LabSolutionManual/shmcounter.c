#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>

int main(int argc, char *argv[]) {
    int procs = 0, iters = 0;
    char *name = NULL;

    // 1. Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--procs") == 0) procs = atoi(argv[++i]);
        else if (strcmp(argv[i], "--iters") == 0) iters = atoi(argv[++i]);
        else if (strcmp(argv[i], "--name") == 0) name = argv[++i];
    }

    // Validation: Check ranges for procs and iters
    if (procs < 2 || procs > 16) {
        fprintf(stderr, "ERROR: E_RANGE: procs must be in 2..16\n");
        return 1;
    }
    if (iters < 1 || iters > 100000) {
        fprintf(stderr, "ERROR: E_RANGE\n");
        return 1;
    }

    // 2. Setup Shared Memory
    char shm_path[64], sem_path[64];
    sprintf(shm_path, "/shm_%s", name);
    sprintf(sem_path, "/sem_%s", name);

    int shm_fd = shm_open(shm_path, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(long)); // Size for one 64-bit integer
    long *counter = mmap(0, sizeof(long), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    *counter = 0; // Initialize counter to 0

    // 3. Setup Semaphore
    sem_t *sem = sem_open(sem_path, O_CREAT, 0666, 1);

    // 4. Fork child processes
    for (int i = 0; i < procs; i++) {
        if (fork() == 0) {
            for (int j = 0; j < iters; j++) {
                sem_wait(sem);   // Lock
                (*counter)++;    // Critical Section
                sem_post(sem);   // Unlock
            }
            exit(0);
        }
    }

    // 5. Parent waits for all children
    for (int i = 0; i < procs; i++) wait(NULL);

    // Output final value: expected is procs * iters
    printf("OK: FINAL %ld\n", *counter);

    // 6. Cleanup resources
    munmap(counter, sizeof(long));
    shm_unlink(shm_path);
    sem_close(sem);
    sem_unlink(sem_path);

    return 0;
}
