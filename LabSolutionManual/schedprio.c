#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char pid[32];
    int arrival, burst, priority, wait, tat, completed;
} Process;

int main() {
    char line[128];
    Process p[64];
    int n = 0;

    // 1. Read CSV Header and Data
    if (!fgets(line, sizeof(line), stdin)) return 0;
    while (fgets(line, sizeof(line), stdin)) {
        if (sscanf(line, "%[^,],%d,%d,%d", p[n].pid, &p[n].arrival, &p[n].burst, &p[n].priority) == 4) {
            // Priority range validation
            if (p[n].priority < 0 || p[n].priority > 99) {
                fprintf(stderr, "ERROR: E_RANGE: priority must be in 0..99\n");
                return 1;
            }
            p[n].completed = 0;
            n++;
        }
    }

    if (n == 0) return 0;

    printf("ALG: PRIO_AGING\n");
    printf("GANTT ");

    int current_time = 0, finished = 0;

    // 2. Simulation Loop
    while (finished < n) {
        int idx = -1;
        int min_eff_prio = 999;

        for (int i = 0; i < n; i++) {
            if (!p[i].completed && p[i].arrival <= current_time) {
                // Apply Aging: Effective Prio = Base - WaitTime
                int wait_time = current_time - p[i].arrival;
                int eff_prio = p[i].priority - wait_time;
                if (eff_prio < 0) eff_prio = 0; // Bound to minimum 0

                if (eff_prio < min_eff_prio) {
                    min_eff_prio = eff_prio;
                    idx = i;
                } else if (eff_prio == min_eff_prio) {
                    // Tie-breaker: earlier arrival
                    if (idx == -1 || p[i].arrival < p[idx].arrival) idx = i;
                }
            }
        }

        if (idx == -1) { // CPU IDLE gap
            int next_arr = 9999;
            for(int i=0; i<n; i++) if(!p[i].completed && p[i].arrival < next_arr) next_arr = p[i].arrival;
            printf("IDLE@%d-%d ", current_time, next_arr);
            current_time = next_arr;
        } else {
            int start = current_time;
            p[idx].wait = current_time - p[idx].arrival;
            current_time += p[idx].burst;
            p[idx].tat = current_time - p[idx].arrival;
            p[idx].completed = 1;
            finished++;
            printf("%s@%d-%d ", p[idx].pid, start, current_time);
        }
    }

    // 3. Output Metrics
    printf("\n");
    double tw = 0, tt = 0;
    for(int i=0; i<n; i++) { tw += p[i].wait; tt += p[i].tat; }
    printf("OK: AVG_WAIT %.2f AVG_TAT %.2f\n", tw/n, tt/n);

    return 0;
}
