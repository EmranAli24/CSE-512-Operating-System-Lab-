#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>     // For directory operations: opendir, readdir
#include <sys/stat.h>   // For file metadata: lstat, S_ISREG, etc.
#include <unistd.h>

// Structure to store entry details for sorting and reporting
typedef struct {
    char name[256];
    long size;
    char type;
} FileEntry;

// Comparator to sort entries by name lexicographically
int compare_name(const void *a, const void *b) {
    return strcmp(((FileEntry *)a)->name, ((FileEntry *)b)->name);
}

// Comparator to sort entries by size (ascending), with name as a tie-break
int compare_size(const void *a, const void *b) {
    FileEntry *fa = (FileEntry *)a;
    FileEntry *fb = (FileEntry *)b;
    if (fa->size != fb->size) return (fa->size - fb->size);
    return strcmp(fa->name, fb->name);
}

int main(int argc, char *argv[]) {
    char *path = NULL;
    char *sort_by = "name"; // Default sorting behavior

    // 1. Parse Command Line Arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--path") == 0) path = argv[++i];
        else if (strcmp(argv[i], "--sort") == 0) sort_by = argv[++i];
    }

    // Basic usage error check
    if (!path) {
        fprintf(stderr, "ERROR: E_USAGE: path is required\n");
        return 1;
    }

    // 2. Open Directory
    DIR *dr = opendir(path);
    if (dr == NULL) {
        struct stat check_st;
        // Check if path exists but is just a file, not a directory
        if (lstat(path, &check_st) == 0 && !S_ISDIR(check_st.st_mode))
            fprintf(stderr, "ERROR: E_NOTDIR: path is not a directory\n");
        else
            fprintf(stderr, "ERROR: E_NOTDIR: path does not exist\n");
        return 1;
    }

    struct dirent *de;
    FileEntry entries[1024]; // Array to hold entries for sorting
    int count = 0, files = 0, dirs = 0, links = 0, other = 0;

    // 3. Traverse Directory Entries
    while ((de = readdir(dr)) != NULL) {
        // Exclude "." (current) and ".." (parent) directories as per hints
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;

        char full_path[1024];
        sprintf(full_path, "%s/%s", path, de->d_name);

        struct stat st;
        // Collect metadata using lstat (does not follow symlinks)
        if (lstat(full_path, &st) == 0) {
            strcpy(entries[count].name, de->d_name);
            entries[count].size = st.st_size;

            // Classify entry types (F=File, D=Dir, L=Link, O=Other)
            if (S_ISREG(st.st_mode)) { entries[count].type = 'F'; files++; }
            else if (S_ISDIR(st.st_mode)) { entries[count].type = 'D'; dirs++; }
            else if (S_ISLNK(st.st_mode)) { entries[count].type = 'L'; links++; }
            else { entries[count].type = 'O'; other++; }
            count++;
        }
    }
    closedir(dr); // Close the directory stream

    // 4. Sort the Collected Entries
    if (strcmp(sort_by, "size") == 0)
        qsort(entries, count, sizeof(FileEntry), compare_size);
    else
        qsort(entries, count, sizeof(FileEntry), compare_name);

    // 5. Output Results
    for (int i = 0; i < count; i++) {
        printf("ENTRY %c %ld %s\n", entries[i].type, entries[i].size, entries[i].name);
    }

    // Print final summary line
    printf("OK: TOTAL %d FILES %d DIRS %d LINKS %d OTHER %d\n", count, files, dirs, links, other);

    return 0;
}
