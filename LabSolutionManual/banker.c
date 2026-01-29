#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main() {
    int P, R;

    // 1. Read process (P) and resource (R) counts
    if (scanf("%d %d", &P, &R) != 2) return 0;

    int alloc[P][R], max[P][R], avail[R], need[P][R];
    bool finished[P];

    // Read Allocation Matrix
    for (int i = 0; i < P; i++) {
        for (int j = 0; j < R; j++) {
            scanf("%d", &alloc[i][j]);
        }
        finished[i] = false;
    }

    // Read Max Matrix and Validate
    for (int i = 0; i < P; i++) {
        for (int j = 0; j < R; j++) {
            scanf("%d", &max[i][j]);
            // Invalid Input Check: Allocation cannot exceed Max
            if (alloc[i][j] > max[i][j]) {
                fprintf(stderr, "ERROR: E_INVALID: allocation must be <= max\n");
                return 1;
            }
            // Calculate Need Matrix
            need[i][j] = max[i][j] - alloc[i][j];
        }
    }

    // Read Available Vector
    for (int i = 0; i < R; i++) {
        scanf("%d", &avail[i]);
    }

    // 2. Safety Algorithm Implementation
    int safe_seq[P], count = 0;
    int work[R];
    for (int i = 0; i < R; i++) work[i] = avail[i];

    while (count < P) {
        bool found = false;
        // Search lexicographically (smallest index first)
        for (int p = 0; p < P; p++) {
            if (!finished[p]) {
                bool can_exec = true;
                for (int j = 0; j < R; j++) {
                    if (need[p][j] > work[j]) {
                        can_exec = false;
                        break;
                    }
                }

                if (can_exec) {
                    // Resource Release: Work = Work + Allocation
                    for (int j = 0; j < R; j++) {
                        work[j] += alloc[p][j];
                    }
                    safe_seq[count++] = p;
                    finished[p] = true;
                    found = true;
                    // Restart search from smallest index for lexicographical smallest sequence
                    break; 
                }
            }
        }
        if (!found) break; // System entered Unsafe State
    }

    // 3. Final Output
    if (count == P) {
        printf("OK: SAFE\n");
        printf("OK: SEQ");
        for (int i = 0; i < P; i++) {
            printf(" %d", safe_seq[i]);
        }
        printf("\n");
    } else {
        printf("OK: UNSAFE\n");
    }

    return 0;
}
