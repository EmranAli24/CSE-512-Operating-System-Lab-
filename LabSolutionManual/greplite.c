#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char *pattern = NULL;
    char *file_list = NULL;

    // 1. Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--pattern") == 0) {
            pattern = argv[++i];
        } else if (strcmp(argv[i], "--files") == 0) {
            file_list = argv[++i];
        }
    }

    // Basic usage and empty pattern error checking
    if (pattern == NULL || file_list == NULL) {
        fprintf(stderr, "ERROR: E_USAGE: missing arguments\n");
        return 1;
    }
    if (strlen(pattern) == 0) {
        fprintf(stderr, "ERROR: E_EMPTY_PATTERN: pattern must be non-empty\n");
        return 1;
    }

    int total_matches = 0;
    int total_files = 0;

    // 2. Split the comma-separated file list
    char *file_name = strtok(file_list, ",");
    while (file_name != NULL) {
        // Reject empty file segments (e.g., trailing comma)
        if (strlen(file_name) == 0) {
            file_name = strtok(NULL, ",");
            continue;
        }

        // 3. Open the file
        FILE *fp = fopen(file_name, "r");
        if (fp == NULL) {
            fprintf(stderr, "ERROR: E_OPEN: could not open file %s\n", file_name);
            return 1;
        }

        char line[1024];
        int line_no = 1;
        total_files++;

        // 4. Read file line by line
        while (fgets(line, sizeof(line), fp)) {
            // Remove the trailing newline character for clean output
            line[strcspn(line, "\n")] = 0;

            // 5. Search for the pattern in the current line
            if (strstr(line, pattern) != NULL) {
                printf("MATCH %s:%d:%s\n", file_name, line_no, line);
                total_matches++;
            }
            line_no++;
        }

        fclose(fp);
        file_name = strtok(NULL, ","); // Get next file from the list
    }

    // 6. Output final summary
    printf("OK: MATCHES %d FILES %d\n", total_matches, total_files);

    return 0;
}
