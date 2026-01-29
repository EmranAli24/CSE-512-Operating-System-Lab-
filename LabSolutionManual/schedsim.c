#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structure to store process details
typedef struct {
    char pid[32];
    int arrival;
    int burst;
    int wait;
    int tat;
    int completed;
} Process;

// Helper to print average metrics with 2 decimal places
void print_metrics(Process p[], int n) {
    double total_wait = 0, total_tat = 0;
    for (int i = 0; i < n; i++) {
        total_wait += p[i].wait;
        total_tat += p[i].tat;
    }
    printf("OK: AVG_WAIT %.2f AVG_TAT %.2f\n", total_wait / n, total_tat / n);
}

// FCFS (First-Come First-Served) Simulation
void simulate_fcfs(Process p_in[], int n) {
    Process p[64];
    memcpy(p, p_in, n * sizeof(Process));
    printf("ALG: FCFS\n");
    printf("GANTT ");

    int current_time = 0;
    for (int i = 0; i < n; i++) {
        if (current_time < p[i].arrival) {
            printf("IDLE@%d-%d ", current_time, p[i].arrival);
            current_time = p[i].arrival;
        }
        int start = current_time;
        current_time += p[i].burst;
        p[i].tat = current_time - p[i].arrival;
        p[i].wait = p[i].tat - p[i].burst;
        printf("%s@%d-%d ", p[i].pid, start, current_time);
    }
    printf("\n");
    print_metrics(p, n);
}

// SJF (Shortest Job First - Non-preemptive) Simulation
void simulate_sjf(Process p_in[], int n) {
    Process p[64];
    memcpy(p, p_in, n * sizeof(Process));
    for(int i=0; i<n; i++) p[i].completed = 0;

    printf("ALG: SJF\n");
    printf("GANTT ");

    int current_time = 0, completed_count = 0;
    while (completed_count < n) {
        int idx = -1;
        int min_burst = 999999;

        // Selection rule: Shortest burst among arrived processes
        for (int i = 0; i < n; i++) {
            if (!p[i].completed && p[i].arrival <= current_time) {
                if (p[i].burst < min_burst) {
                    min_burst = p[i].burst;
                    idx = i;
                } else if (p[i].burst == min_burst) {
                    // Tie-breaker: earlier arrival
                    if (p[i].arrival < p[idx].arrival) idx = i;
                }
            }
        }

        if (idx == -1) {
            // Find the next arriving process for IDLE time
            int next_arrival = 999999;
            for(int i=0; i<n; i++) if(!p[i].completed && p[i].arrival < next_arrival) next_arrival = p[i].arrival;
            printf("IDLE@%d-%d ", current_time, next_arrival);
            current_time = next_arrival;
        } else {
            int start = current_time;
            current_time += p[idx].burst;
            p[idx].tat = current_time - p[idx].arrival;
            p[idx].wait = p[idx].tat - p[idx].burst;
            p[idx].completed = 1;
            completed_count++;
            printf("%s@%d-%d ", p[idx].pid, start, current_time);
        }
    }
    printf("\n");
    print_metrics(p, n);
}

int main() {
    char line[128];
    Process p[64];
    int n = 0;

    // 1. Read CSV Header
    if (!fgets(line, sizeof(line), stdin)) return 0;

    // 2. Read and Validate Process Data
    while (fgets(line, sizeof(line), stdin)) {
        if (strlen(line) <= 1) continue;
        if (sscanf(line, "%[^,],%d,%d", p[n].pid, &p[n].arrival, &p[n].burst) == 3) {
            // Validation per snapshot requirement
            if (p[n].arrival < 0 || p[n].burst <= 0) {
                fprintf(stderr, "ERROR: E_RANGE: arrival and burst must be non-negative; burst must be > 0\n");
                return 1;
            }
            p[n].completed = 0;
            n++;
        }
    }

    if (n == 0) return 0;

    // 3. Pre-sort by Arrival Time for baseline order
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (p[j].arrival > p[j+1].arrival) {
                Process temp = p[j]; p[j] = p[j+1]; p[j+1] = temp;
            }
        }
    }

    // 4. Run Simulations
    simulate_fcfs(p, n);
    printf("\n");
    simulate_sjf(p, n);

    return 0;
}
