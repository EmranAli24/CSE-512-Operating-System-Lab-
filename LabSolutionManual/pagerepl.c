#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_result(char* alg, int faults, int frames[], int F) {
    printf("ALG %s\n", alg);
    printf("OK: FAULTS %d\n", faults);
    printf("OK: FINAL");
    for (int i = 0; i < F; i++) printf(" %d", frames[i]);
    printf("\n\n");
}

int find_in_frames(int page, int frames[], int F) {
    for (int i = 0; i < F; i++) if (frames[i] == page) return i;
    return -1;
}

// FIFO: First-In First-Out
void solve_fifo(int L, int refs[], int F) {
    int frames[F], faults = 0, pointer = 0;
    for (int i = 0; i < F; i++) frames[i] = -1; // Empty frames

    for (int i = 0; i < L; i++) {
        if (find_in_frames(refs[i], frames, F) == -1) {
            faults++; // Increment fault even if frames are empty
            frames[pointer] = refs[i];
            pointer = (pointer + 1) % F;
        }
    }
    print_result("FIFO", faults, frames, F);
}

// LRU: Least Recently Used
void solve_lru(int L, int refs[], int F) {
    int frames[F], last_used[F], faults = 0;
    for (int i = 0; i < F; i++) { frames[i] = -1; last_used[i] = -1; }

    for (int i = 0; i < L; i++) {
        int idx = find_in_frames(refs[i], frames, F);
        if (idx != -1) {
            last_used[idx] = i; // Hit: update time
        } else {
            faults++; // Miss: add to fault
            int victim = 0;
            // Rule: prioritize smallest index for empty frames
            for (int j = 0; j < F; j++) {
                if (frames[j] == -1) { victim = j; break; }
                if (last_used[j] < last_used[victim]) victim = j;
            }
            frames[victim] = refs[i];
            last_used[victim] = i;
        }
    }
    print_result("LRU", faults, frames, F);
}

// OPT: Optimal Page Replacement
void solve_opt(int L, int refs[], int F) {
    int frames[F], faults = 0;
    for (int i = 0; i < F; i++) frames[i] = -1;

    for (int i = 0; i < L; i++) {
        if (find_in_frames(refs[i], frames, F) == -1) {
            faults++; // Miss: add to fault
            int victim = -1;
            for (int j = 0; j < F; j++) {
                if (frames[j] == -1) { victim = j; break; }
            }
            if (victim == -1) {
                int farthest = -1;
                for (int j = 0; j < F; j++) {
                    int next_use = 1000000;
                    for (int k = i + 1; k < L; k++) {
                        if (refs[k] == frames[j]) { next_use = k; break; }
                    }
                    if (next_use > farthest) { farthest = next_use; victim = j; }
                }
            }
            frames[victim] = refs[i];
        }
    }
    print_result("OPT", faults, frames, F);
}

int main(int argc, char *argv[]) {
    int F = 0;
    for (int i = 1; i < argc; i++) if (strcmp(argv[i], "--frames") == 0) F = atoi(argv[++i]);
    if (F < 1 || F > 64) {
        fprintf(stderr, "ERROR: E_RANGE: frames must be 1..64\n");
        return 1;
    }
    int L; if (scanf("%d", &L) != 1) return 0;
    int refs[L];
    for (int i = 0; i < L; i++) {
        scanf("%d", &refs[i]);
        if (refs[i] < 0) {
            fprintf(stderr, "ERROR: E_RANGE: page numbers must be >= 0\n");
            return 1;
        }
    }
    solve_fifo(L, refs, F);
    solve_lru(L, refs, F);
    solve_opt(L, refs, F);
    return 0;
}
