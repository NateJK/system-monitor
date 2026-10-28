#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>

// Start of the source c code for system_monitor application

// Defines the data we want to track for each process
struct Process {
    int pid;
    int memory_kb;
    char name[16];
};

int is_numeric(const char *name) {
    if (*name == '\0') {
        return 0;
    }

    while (*name != '\0') {
        if(!isdigit((unsigned char)*name)) {
            return 0;
        }

        name++;
    }

    return 1;
}

int main(void) {

    char line[256];

    DIR *proc_dir = opendir("/proc");

    if (proc_dir == NULL) {
        perror("opendir");
        return 1;
    }

    struct dirent *entry;

    char path[256];

    while ((entry = readdir(proc_dir)) != NULL) {
        if (is_numeric(entry->d_name)) {
            snprintf(path, sizeof(path), "/proc/%s/status", entry->d_name);

            FILE *process_info;
            process_info = fopen(path, "r");

            if (process_info == NULL) {
                perror("fopen");

                return 1;
            }

            struct Process current_process = {0};

            int found_memory = 0;
            int found_pid = 0;
            int found_name = 0;

            while (fgets(line, sizeof(line), process_info) != NULL) {
                if (strncmp(line, "Pid:", 4) == 0) {
                    if (sscanf(line, "Pid: %d", &current_process.pid) == 1) {
                        found_pid = 1;
                    }
                } else if (strncmp(line, "VmRSS:", 6) == 0) {
                    if (sscanf(line, "VmRSS: %d kB", &current_process.memory_kb) == 1) {
                        found_memory = 1;
                    }
                } else if (strncmp(line, "Name:", 5) == 0) {
                    if (sscanf(line, "Name: %15s", current_process.name) == 1) {
                        found_name = 1;
                    }
                }
            }

            if (found_memory && found_pid && 0) {
                printf("Name: %s | PID: %d | Memory: %d kB\n", current_process.name, current_process.pid, current_process.memory_kb);
            }

            fclose(process_info);
        }
    }

    closedir(proc_dir);

    return 0;
}