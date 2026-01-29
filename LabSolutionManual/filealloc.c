#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char name[50]; int size; } FileInfo;

// 1. Contiguous Allocation
void solve_contiguous(int N, int F, int free_list[], int M, FileInfo files[]) {
    printf("ALG CONTIGUOUS\n");
    int disk[N]; 
    for(int i=0; i<N; i++) disk[i] = 0;
    for(int i=0; i<F; i++) disk[free_list[i]] = 1;

    for(int i=0; i<M; i++) {
        int start = -1;
        for(int j=0; j <= N - files[i].size; j++) {
            int ok = 1;
            for(int k=0; k < files[i].size; k++) if(disk[j+k] == 0) { ok = 0; break; }
            if(ok) { start = j; break; }
        }
        if(start != -1) {
            printf("FILE %s -> START %d LEN %d\n", files[i].name, start, files[i].size);
            for(int k=0; k < files[i].size; k++) disk[start+k] = 0;
        } else printf("FILE %s -> FAIL\n", files[i].name);
    }
    printf("\n");
}

// 2. Linked Allocation
void solve_linked(int N, int F, int free_list[], int M, FileInfo files[]) {
    printf("ALG LINKED\n");
    int disk_free[F], used[F];
    for(int i=0; i<F; i++) { disk_free[i] = free_list[i]; used[i] = 0; }
    int current_free_ptr = 0;

    for(int i=0; i<M; i++) {
        int available = 0;
        for(int j=0; j<F; j++) if(!used[j]) available++;

        if(available >= files[i].size) {
            printf("FILE %s -> CHAIN", files[i].name);
            int count = 0;
            for(int j=0; j<F && count < files[i].size; j++) {
                if(!used[j]) {
                    printf("%s%d", (count == 0 ? " " : "->"), disk_free[j]);
                    used[j] = 1;
                    count++;
                }
            }
            printf("\n");
        } else printf("FILE %s -> FAIL\n", files[i].name);
    }
    printf("\n");
}

// 3. Indexed Allocation
void solve_indexed(int N, int F, int free_list[], int M, FileInfo files[]) {
    printf("ALG INDEXED\n");
    int disk_free[F], used[F];
    for(int i=0; i<F; i++) { disk_free[i] = free_list[i]; used[i] = 0; }

    for(int i=0; i<M; i++) {
        // Need size + 1 (for index block)
        int needed = files[i].size + 1;
        int available = 0;
        for(int j=0; j<F; j++) if(!used[j]) available++;

        if(available >= needed) {
            int index_block = -1;
            for(int j=0; j<F; j++) if(!used[j]) { index_block = disk_free[j]; used[j] = 1; break; }
            
            printf("FILE %s -> INDEX %d DATA", files[i].name, index_block);
            int count = 0;
            for(int j=0; j<F && count < files[i].size; j++) {
                if(!used[j]) {
                    printf("%s%d", (count == 0 ? " " : ","), disk_free[j]);
                    used[j] = 1;
                    count++;
                }
            }
            printf("\n");
        } else printf("FILE %s -> FAIL\n", files[i].name);
    }
}

int main() {
    int N, F, M;
    if (scanf("%d %d", &N, &F) != 2) return 0;
    int free_blocks[F], check[1000] = {0};
    for (int i = 0; i < F; i++) {
        scanf("%d", &free_blocks[i]);
        if (free_blocks[i] < 0 || free_blocks[i] >= N || check[free_blocks[i]]) {
            fprintf(stderr, "ERROR: E_DUPLICATE: free block list must contain unique IDs\n");
            return 1;
        }
        check[free_blocks[i]] = 1;
    }
    if (scanf("%d", &M) != 1) return 0;
    FileInfo files[M];
    for (int i = 0; i < M; i++) scanf("%s %d", files[i].name, &files[i].size);

    solve_contiguous(N, F, free_blocks, M, files);
    solve_linked(N, F, free_blocks, M, files);
    solve_indexed(N, F, free_blocks, M, files);

    return 0;
}
