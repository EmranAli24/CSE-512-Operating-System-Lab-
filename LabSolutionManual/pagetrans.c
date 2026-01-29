#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int vpn, pfn, valid; } PageTableEntry;
typedef struct { int vpn, pfn, active; } TLBEntry;

int main(int argc, char *argv[]) {
    int page_size = 0, tlb_size = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--pagesize") == 0) page_size = atoi(argv[++i]);
        else if (strcmp(argv[i], "--tlb") == 0) tlb_size = atoi(argv[++i]);
    }

    // Invalid Case: Power of 2 Check
    if (page_size < 2 || (page_size & (page_size - 1)) != 0) {
        fprintf(stderr, "ERROR: E_RANGE: page size must be power of 2\n");
        return 1;
    }

    int N, Q;
    if (scanf("%d", &N) != 1) return 0;
    PageTableEntry pt[N];
    for (int i = 0; i < N; i++) scanf("%d %d %d", &pt[i].vpn, &pt[i].pfn, &pt[i].valid);

    TLBEntry tlb[tlb_size];
    for (int i = 0; i < tlb_size; i++) tlb[i].active = 0;

    int hits = 0, misses = 0;
    if (scanf("%d", &Q) != 1) return 0;
    while (Q--) {
        unsigned int vaddr;
        scanf("%u", &vaddr);
        int vpn = vaddr / page_size;
        int offset = vaddr % page_size;

        // TLB Logic (Optional)
        int tlb_hit = 0;
        if (tlb_size > 0) {
            int idx = vpn % tlb_size;
            if (tlb[idx].active && tlb[idx].vpn == vpn) {
                printf("OK: VA %u -> PA %d (TLB HIT)\n", vaddr, tlb[idx].pfn * page_size + offset);
                hits++; tlb_hit = 1;
            } else misses++;
        }
        if (tlb_hit) continue;

        // Page Table Logic
        int found_idx = -1;
        for (int j = 0; j < N; j++) {
            if (pt[j].vpn == vpn) { found_idx = j; break; }
        }

        // Invalid Case Check: Not found or Valid Bit 0
        if (found_idx != -1 && pt[found_idx].valid) {
            int paddr = pt[found_idx].pfn * page_size + offset;
            if (tlb_size > 0) {
                int idx = vpn % tlb_size;
                tlb[idx] = (TLBEntry){vpn, pt[found_idx].pfn, 1};
                printf("OK: VA %u -> PA %d (TLB MISS)\n", vaddr, paddr);
            } else printf("OK: VA %u -> PA %d\n", vaddr, paddr);
        } else {
            printf("OK: VA %u -> PAGE FAULT\n", vaddr);
        }
    }
    if (tlb_size > 0) printf("OK: TLB_HITS %d TLB_MISSES %d\n", hits, misses);
    return 0;
}
