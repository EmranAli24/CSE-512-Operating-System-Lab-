#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int is_valid_octal(const char *s) {
    if (strlen(s) != 4) return 0;
    for (int i = 0; i < 4; i++) {
        if (s[i] < '0' || s[i] > '7')
            return 0;
    }
    return 1;
}

void octal_to_symbolic(int perm, char *out) {
    const char symbols[] = {'r','w','x'};
    int pos = 0;

    for (int i = 2; i >= 0; i--) {
        int val = (perm >> (i * 3)) & 7;
        for (int j = 0; j < 3; j++) {
            out[pos++] = (val & (1 << (2 - j))) ? symbols[j] : '-';
        }
    }
    out[pos] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc != 3 && argc != 5) {
        printf("ERROR: E_USAGE: invalid arguments\n");
        return 1;
    }

    if (strcmp(argv[1], "--mode") != 0) {
        printf("ERROR: E_USAGE: missing --mode\n");
        return 1;
    }

    if (!is_valid_octal(argv[2])) {
        printf("ERROR: E_OCTAL: mode must be 4-digit octal (0000-0777)\n");
        return 1;
    }

    int mode = strtol(argv[2], NULL, 8);
    int umask = 0;

    if (argc == 5) {
        if (strcmp(argv[3], "--umask") != 0 || !is_valid_octal(argv[4])) {
            printf("ERROR: E_OCTAL: invalid umask\n");
            return 1;
        }
        umask = strtol(argv[4], NULL, 8);
    }

    int effective = mode & (~umask & 0777);

    char symbolic[10];
    octal_to_symbolic(effective, symbolic);

    printf("OK: EFFECTIVE %04o\n", effective);
    printf("OK: SYMBOLIC %s\n", symbolic);

    return 0;
}

