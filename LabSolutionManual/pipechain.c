#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h> // Required for open() and O_WRONLY

// Helper function to parse comma-separated arguments
void parse_args(char *cmd, char *args_str, char **exec_args) {
    exec_args[0] = cmd;
    int idx = 1;
    if (args_str) {
        char *token = strtok(args_str, ",");
        while (token != NULL) {
            exec_args[idx++] = token;
            token = strtok(NULL, ",");
        }
    }
    exec_args[idx] = NULL; // NULL terminate the argument list
}

int main(int argc, char *argv[]) {
    char *stages_cmd[3] = {NULL, NULL, NULL};
    char *stages_args[3] = {NULL, NULL, NULL};
    char *stage_names[3] = {"producer", "filter", "consumer"};

    // 1. Parsing command line flags
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--producer") == 0) stages_cmd[0] = argv[++i];
        else if (strcmp(argv[i], "--producer-args") == 0) stages_args[0] = argv[++i];
        else if (strcmp(argv[i], "--filter") == 0) stages_cmd[1] = argv[++i];
        else if (strcmp(argv[i], "--filter-args") == 0) stages_args[1] = argv[++i];
        else if (strcmp(argv[i], "--consumer") == 0) stages_cmd[2] = argv[++i];
        else if (strcmp(argv[i], "--consumer-args") == 0) stages_args[2] = argv[++i];
    }

    if (!stages_cmd[0] || !stages_cmd[1] || !stages_cmd[2]) {
        fprintf(stderr, "ERROR: E_USAGE\n");
        return 1;
    }

    int pipe1[2], pipe2[2];
    pipe(pipe1); // Create first pipe
    pipe(pipe2); // Create second pipe

    pid_t pids[3];

    // 2. Launching stages
    for (int i = 0; i < 3; i++) {
        pids[i] = fork();
        if (pids[i] == 0) {
            // --- Inside Child Process ---
            
            // Redirect Pipe Ends
            if (i == 0) { // Producer writes to pipe1
                dup2(pipe1[1], STDOUT_FILENO);
            } else if (i == 1) { // Filter reads from pipe1, writes to pipe2
                dup2(pipe1[0], STDIN_FILENO);
                dup2(pipe2[1], STDOUT_FILENO);
            } else if (i == 2) { // Consumer reads from pipe2
                dup2(pipe2[0], STDIN_FILENO);
                
                // Deterministic Rule: Redirect final output to /dev/null
                int dev_null = open("/dev/null", O_WRONLY);
                dup2(dev_null, STDOUT_FILENO);
                dup2(dev_null, STDERR_FILENO);
                close(dev_null);
            }

            // Close all pipe file descriptors in the child
            close(pipe1[0]); close(pipe1[1]);
            close(pipe2[0]); close(pipe2[1]);

            char *exec_args[64];
            parse_args(stages_cmd[i], stages_args[i], exec_args);
            execvp(stages_cmd[i], exec_args); // Replace process image
            exit(127); 
        }
    }

    // 3. Parent Cleanup and Status Collection
    close(pipe1[0]); close(pipe1[1]);
    close(pipe2[0]); close(pipe2[1]);

    int status, first_fail_idx = -1;
    int fail_code = 0, is_sig = 0;

    // Wait for all stages and record the first failure
    for (int i = 0; i < 3; i++) {
        waitpid(pids[i], &status, 0);
        if (first_fail_idx == -1) {
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                first_fail_idx = i; fail_code = WEXITSTATUS(status);
            } else if (WIFSIGNALED(status)) {
                first_fail_idx = i; fail_code = WTERMSIG(status); is_sig = 1;
            }
        }
    }

    // 4. Print Final Report
    if (first_fail_idx == -1) {
        printf("OK: PIPELINE SUCCESS\n"); // Matches Sample I/O
    } else {
        if (is_sig) printf("ERROR: E_STAGE: stage %s sig %d\n", stage_names[first_fail_idx], fail_code);
        else printf("ERROR: E_STAGE: stage %s exit %d\n", stage_names[first_fail_idx], fail_code);
    }

    return 0;
}
