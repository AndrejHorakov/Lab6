#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>

void printProcessInfo(const char *processName) {
    pid_t pid = getpid();
    pid_t parent_pid = getppid();

    struct timeval ts;
    struct timezone tz;
    gettimeofday (&ts, &tz);
    printf("%s - PID: %d, PPID: %d, Current Time: %02d:%02d:%02d:%03d\n", processName, pid, parent_pid,
           (int)((ts.tv_sec + 10800) / 3600 % 24), (int)(ts.tv_sec / 60 % 60), (int)(ts.tv_sec % 60), (int)(ts.tv_usec / 1000));
}

int main() {
    pid_t child1, child2;

    child1 = fork();

    if (child1 == 0) {
        printProcessInfo("Child 1");
    } else {
        child2 = fork();
        if (child2 == 0) {
            printProcessInfo("Child 2");
        } else {
            printProcessInfo("Parent");
            system("ps -x");
            
            wait(NULL);
            wait(NULL);
        }
    }

    return 0;
}
