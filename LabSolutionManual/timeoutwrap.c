#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

// Global variable to store child process ID so the signal handler can access it
pid_t child_pid = -1; 

// Signal handler: This function runs when the alarm goes off
void handle_alarm(int sig) {
    if (child_pid > 0) {
        // Forcefully kill the child process if it's still running
        kill(child_pid, SIGKILL); 
    }
}

int main(int argc, char *argv[]) {
    int seconds = 0;
    char *cmd = NULL;
    char *args_str = NULL;

    // 1. Parse Command Line Arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--seconds") == 0) seconds = atoi(argv[++i]);
        else if (strcmp(argv[i], "--cmd") == 0) cmd = argv[++i];
        else if (strcmp(argv[i], "--args") == 0) args_str = argv[++i];
    }

    // Validation: Check if seconds is within the required range (1-60)
    if (seconds < 1 || seconds > 60) {
        fprintf(stderr, "ERROR: E_RANGE: seconds must be in 1..60\n");
        return 1;
    }
    if (!cmd) {
        fprintf(stderr, "ERROR: E_USAGE: --cmd is required\n");
        return 1;
    }

    // Set up sigaction for SIGALRM for more reliable signal handling
    struct sigaction sa;
    sa.sa_handler = handle_alarm;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);

    // 2. Prepare arguments for execvp
    char *exec_args[64];
    exec_args[0] = cmd;
    int arg_idx = 1;

    if (args_str) {
        // Use strtok to split the comma-separated arguments
        char *token = strtok(args_str, ",");
        while (token != NULL) {
            exec_args[arg_idx++] = token;
            token = strtok(NULL, ",");
        }
    }
    exec_args[arg_idx] = NULL; // Array must be NULL-terminated

    // 3. Fork and Monitor
    child_pid = fork(); 

    if (child_pid == 0) {
        // Inside Child Process: Try to run the command
        execvp(cmd, exec_args);
        exit(127); // Exit with code 127 if execvp fails
    } else {
        // Inside Parent Process: Start the timer
        alarm(seconds);

        int status;
        // Wait for child to finish or be killed by signal
        waitpid(child_pid, &status, 0); 
        alarm(0); // Cancel the alarm if child finished within time

        // 4. Output Logic: Reporting how the process ended
        if (WIFSIGNALED(status)) {
            // If the process was terminated by any signal after the timeout
            // we report it as TIMEOUT KILLED to match lab specifications
            printf("OK: TIMEOUT KILLED\n");
        } else if (WIFEXITED(status)) {
            // If the process finished normally, report its exit code
            printf("OK: EXIT %d\n", WEXITSTATUS(status));
        }
    }

    return 0;
}
