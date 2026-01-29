#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    char *cmd = NULL;
    char *args_str = NULL;
    int repeat = 1;

    // 1. Parse Command Line Arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--cmd") == 0) cmd = argv[++i];
        else if (strcmp(argv[i], "--args") == 0) args_str = argv[++i];
        else if (strcmp(argv[i], "--repeat") == 0) repeat = atoi(argv[++i]);
    }

    // Validation: cmd is required and repeat must be >= 1
    if (!cmd) {
        fprintf(stderr, "ERROR: E_USAGE\n");
        return 1;
    }
    if (repeat < 1) {
        fprintf(stderr, "ERROR: E_RANGE\n");
        return 1;
    }

    // Prepare arguments array for execvp
    char *exec_args[64];
    exec_args[0] = cmd;
    int arg_idx = 1;
    if (args_str) {
        char *token = strtok(args_str, ",");
        while (token != NULL) {
            exec_args[arg_idx++] = token;
            token = strtok(NULL, ",");
        }
    }
    exec_args[arg_idx] = NULL; // Array must be NULL-terminated

    // 2. Spawn processes sequentially
    for (int i = 1; i <= repeat; i++) {
        pid_t pid = fork(); // Create child process

        if (pid < 0) {
            fprintf(stderr, "ERROR: E_FORK\n");
            return 1;
        }

        if (pid == 0) {
            // --- Inside Child Process ---
            // If execvp succeeds, the code below it never runs
            execvp(cmd, exec_args);
            
            // If exec fails, exit with code 127 to signal the parent
            exit(127); 
        } else {
            // --- Inside Parent Process ---
            int status;
            // Wait for child to terminate and get status
            waitpid(pid, &status, 0);

            if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);
                
                // If it's the first child and exec failed, stop and report ERROR
                if (exit_code == 127 && i == 1) {
                    fprintf(stderr, "ERROR: E_EXEC: cannot exec program\n");
                    return 1;
                }
                
                // If exec was successful, print START and then EXIT
                printf("CHILD %d PID %d START\n", i, pid);
                printf("CHILD %d PID %d EXIT %d\n", i, pid, exit_code);
            } else if (WIFSIGNALED(status)) {
                // Report if child was killed by a signal
                printf("CHILD %d PID %d SIG %d\n", i, pid, WTERMSIG(status));
            }
        }
    }

    // Final success message
    printf("OK: COMPLETED %d\n", repeat);
    return 0;
}
