#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void simulate(char* name, int H, int P, int holes_in[], int procs[]) {
    int holes[H];
    printf("ALG: %s\n", name);
    int allocated_count = 0;

    memcpy(holes, holes_in, H * sizeof(int)); // Original holes for each algorithm

    for (int i = 0; i < P; i++) {
        int best_idx = -1;

        if (strcmp(name, "FIRST_FIT") == 0) {
            for (int j = 0; j < H; j++) {
                if (holes[j] >= procs[i]) {
                    best_idx = j;
                    break; // Pick the first available
                }
            }
        } else if (strcmp(name, "BEST_FIT") == 0) {
            int min_so_far = 1e9;
            for (int j = 0; j < H; j++) {
                if (holes[j] >= procs[i] && holes[j] < min_so_far) {
                    min_so_far = holes[j];
                    best_idx = j;
                }
            }
        } else if (strcmp(name, "WORST_FIT") == 0) {
            int max_so_far = -1;
            for (int j = 0; j < H; j++) {
                if (holes[j] >= procs[i] && holes[j] > max_so_far) {
                    max_so_far = holes[j];
                    best_idx = j;
                }
            }
        }

        if (best_idx != -1) {
            printf("PROC %d SIZE %d -> BLOCK %d\n", i, procs[i], best_idx);
            holes[best_idx] -= procs[i]; // Deduct space
            allocated_count++;
        } else {
            printf("PROC %d SIZE %d -> FAIL\n", i, procs[i]);
        }
    }
    printf("OK: ALLOCATED %d/%d\n", allocated_count, P);
}

int main() {
    int H, P;
    if (scanf("%d %d", &H, &P) != 2) return 0;

    // Range check per snapshot
    if (H <= 0 || P <= 0) {
        fprintf(stderr, "ERROR: E_RANGE: H and P must be positive\n");
        return 1;
    }

    int holes[H], procs[P];
    for (int i = 0; i < H; i++) scanf("%d", &holes[i]);
    for (int i = 0; i < P; i++) scanf("%d", &procs[i]);

    simulate("FIRST_FIT", H, P, holes, procs);
    printf("\n");
    simulate("BEST_FIT", H, P, holes, procs);
    printf("\n");
    simulate("WORST_FIT", H, P, holes, procs);

    return 0;
}
