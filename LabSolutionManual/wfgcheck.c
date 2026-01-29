#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX 100

int adj[MAX][MAX], P, E;
int visited[MAX], path[MAX], parent[MAX];
int cycle_start = -1, cycle_end = -1;

// DFS to detect cycle in a directed graph
bool find_cycle(int u) {
    visited[u] = 1; // Mark as visiting
    for (int v = 0; v < P; v++) {
        if (adj[u][v]) {
            if (visited[v] == 1) { // Cycle detected
                cycle_start = v;
                cycle_end = u;
                return true;
            }
            if (visited[v] == 0) {
                parent[v] = u;
                if (find_cycle(v)) return true;
            }
        }
    }
    visited[u] = 2; // Mark as fully visited
    return false;
}

int main() {
    // 1. Read input
    if (scanf("%d %d", &P, &E) != 2) return 0;

    for (int i = 0; i < E; i++) {
        int u, v;
        scanf("%d %d", &u, &v);
        // Validation: Node range check
        if (u >= P || v >= P || u < 0 || v < 0) {
            fprintf(stderr, "ERROR: E_INPUT: node index out of range\n");
            return 1;
        }
        adj[u][v] = 1;
    }

    // 2. Search for cycle
    bool deadlock = false;
    for (int i = 0; i < P; i++) {
        if (visited[i] == 0) {
            if (find_cycle(i)) {
                deadlock = true;
                break;
            }
        }
    }

    // 3. Output results
    if (deadlock) {
        printf("OK: DEADLOCK YES\n");
        printf("OK: CYCLE");
        
        // Backtrack to find exact cycle nodes
        int curr = cycle_end;
        int res[MAX], k = 0;
        while (curr != cycle_start) {
            res[k++] = curr;
            curr = parent[curr];
        }
        res[k++] = cycle_start;
        for (int i = k - 1; i >= 0; i--) printf(" % d", res[i]);
        printf("\n");
    } else {
        printf("OK: DEADLOCK NO\n");
    }

    return 0;
}
