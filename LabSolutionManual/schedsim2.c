#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char pid[32];
    int arrival, burst, remaining, wait, tat, completed, in_queue;
} Process;

int main(int argc, char *argv[]) {
    int quantum = 0;
    // 1. Parse Quantum Argument
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-q") == 0) quantum = atoi(argv[++i]);
    }

    if (quantum < 1 || quantum > 1000) {
        fprintf(stderr, "ERROR: E_RANGE: quantum must be in 1..1000\n");
        return 1;
    }

    char line[128];
    Process p[64];
    int n = 0;
    fgets(line, sizeof(line), stdin); // Skip header
    while (fgets(line, sizeof(line), stdin)) {
        if (sscanf(line, "%[^,],%d,%d", p[n].pid, &p[n].arrival, &p[n].burst) == 3) {
            p[n].remaining = p[n].burst;
            p[n].completed = 0;
            p[n].in_queue = 0;
            n++;
        }
    }

    // Sort by arrival initially for initial queueing
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (p[j].arrival > p[j+1].arrival) {
                Process temp = p[j]; p[j] = p[j+1]; p[j+1] = temp;
            }
        }
    }

    printf("ALG: RR\n");
    printf("GANTT ");

    int current_time = 0, finished = 0;
    int queue[1000], head = 0, tail = 0;

    // 2. Round Robin Simulation Loop
    while (finished < n) {
        // Add newly arrived processes to queue
        for (int i = 0; i < n; i++) {
            if (!p[i].completed && !p[i].in_queue && p[i].arrival <= current_time) {
                queue[tail++] = i;
                p[i].in_queue = 1;
            }
        }

        if (head == tail) { // CPU is IDLE
            int next_arr = 9999;
            for(int i=0; i<n; i++) if(!p[i].completed && p[i].arrival < next_arr) next_arr = p[i].arrival;
            printf("IDLE@%d-%d ", current_time, next_arr);
            current_time = next_arr;
            continue;
        }

        int idx = queue[head++];
        int execute = (p[idx].remaining < quantum) ? p[idx].remaining : quantum;
        
        printf("%s@%d-%d ", p[idx].pid, current_time, current_time + execute);
        current_time += execute;
        p[idx].remaining -= execute;

        // Check for new arrivals during execution
        for (int i = 0; i < n; i++) {
            if (!p[i].completed && !p[i].in_queue && p[i].arrival <= current_time) {
                queue[tail++] = i;
                p[i].in_queue = 1;
            }
        }

        if (p[idx].remaining > 0) {
            queue[tail++] = idx; // Re-enqueue if not finished
        } else {
            p[idx].completed = 1;
            finished++;
            p[idx].tat = current_time - p[idx].arrival;
            p[idx].wait = p[idx].tat - p[idx].burst;
        }
    }

    printf("\n");
    double tw = 0, tt = 0;
    for(int i=0; i<n; i++) { tw += p[i].wait; tt += p[i].tat; }
    printf("OK: AVG_WAIT %.2f AVG_TAT %.2f\n", tw/n, tt/n);

    return 0;
}
