#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>

#define MAX_BUF 1048576

uint32_t crc32_update(uint32_t crc, unsigned char c) {
    crc ^= c;
    for (int i = 0; i < 8; i++)
        crc = (crc & 1) ? (crc >> 1) ^ 0xEDB88320 : crc >> 1;
    return crc;
}

int main(int argc, char *argv[]) {
    char *src = NULL, *dst = NULL;
    int bufsize = 4096, force = 0;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--src")) src = argv[++i];
        else if (!strcmp(argv[i], "--dst")) dst = argv[++i];
        else if (!strcmp(argv[i], "--buf")) bufsize = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--force")) force = 1;
        else {
            printf("ERROR: E_USAGE: invalid arguments\n");
            return 1;
        }
    }

    if (!src || !dst) {
        printf("ERROR: E_USAGE: missing required arguments\n");
        return 1;
    }

    if (bufsize < 1 || bufsize > MAX_BUF) {
        printf("ERROR: E_RANGE: invalid buffer size\n");
        return 1;
    }

    int fd_src = strcmp(src, "-") == 0 ? STDIN_FILENO : open(src, O_RDONLY);
    if (fd_src < 0) {
        printf("ERROR: E_OPEN_SRC: cannot open source\n");
        return 1;
    }

    int flags = O_WRONLY | O_CREAT;
    if (!force) flags |= O_EXCL;
    flags |= O_TRUNC;

    int fd_dst = open(dst, flags, 0644);
    if (fd_dst < 0) {
        printf("ERROR: E_EXISTS: destination already exists (use --force)\n");
        return 1;
    }

    unsigned char *buf = malloc(bufsize);
    ssize_t r;
    uint64_t total = 0;
    uint32_t crc = 0xFFFFFFFF;

    while ((r = read(fd_src, buf, bufsize)) > 0) {
        for (ssize_t i = 0; i < r; i++)
            crc = crc32_update(crc, buf[i]);

        ssize_t w = 0;
        while (w < r) {
            ssize_t n = write(fd_dst, buf + w, r - w);
            if (n < 0) {
                printf("ERROR: E_WRITE: write failed\n");
                return 1;
            }
            w += n;
        }
        total += r;
    }

    crc ^= 0xFFFFFFFF;

    printf("OK: COPIED %lu BYTES\n", total);
    printf("OK: CRC32 %08x\n", crc);

    close(fd_src);
    close(fd_dst);
    free(buf);
    return 0;
}

