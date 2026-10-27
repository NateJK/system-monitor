#include <stdio.h>
#include <string.h>
#include <dirent.h>

// Start of the source c code for system_monitor application

// Defines the data we want to track for each process
struct Process {
    int pid;
    int memory_kb;
};

int main(void) {

    // Opens the file that contains the system information
    FILE *sys_info;
    sys_info = fopen("/proc/self/status", "r");

    // Error checking if the file is unable to be opened
    if (sys_info == NULL) {
        perror("fopen");

        return 1;
    }

    char line[256];

    struct Process current_process;

    while (fgets(line, sizeof(line), sys_info) != NULL) {
        if (strncmp(line, "Pid:", 4) == 0) {
            if (sscanf(line, "Pid: %d", &current_process.pid) == 1) {
                printf("PID: %d\n", current_process.pid);
            }
        }

        if (strncmp(line, "VmRSS:", 6) == 0) {
            if (sscanf(line, "VmRSS: %d kB", &current_process.memory_kb) == 1) {
                printf("VmRSS: %d\n", current_process.memory_kb);                
            }
        }
    }

    fclose(sys_info);

    DIR *proc_dir = opendir("/proc");

    if (proc_dir == NULL) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;

    while ((entry = readdir(proc_dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    closedir(proc_dir);

    return 0;
}