#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <stdlib.h>
#include <stddef.h>

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

int compare_memory(const void *a, const void *b) {
    const struct Process *process_a = (const struct Process *)a;
    const struct Process *process_b = (const struct Process *)b;

    if (process_a->memory_kb > process_b->memory_kb) {
        return -1;
    } else if (process_a->memory_kb < process_b->memory_kb) {
        return 1;
    } else {
        return 0;
    }
}

int main(void) {

    struct Process *processes = NULL;
    size_t process_count = 0;
    size_t capacity = 0;

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

            if (found_memory && found_pid && found_name) {
                // printf("Name: %s | PID: %d | Memory: %d kB\n", current_process.name, current_process.pid, current_process.memory_kb);

                if (capacity == process_count) {
                    if (capacity == 0) {
                        capacity = 16;
                    } else {
                        capacity *= 2; 
                    }

                    struct Process *temp = realloc(processes, capacity * sizeof(struct Process));

                    if (temp == NULL) {
                        free(processes);
                        fclose(process_info);
                        closedir(proc_dir);
                        perror("realloc");

                        return 1;
                    } else {
                        processes = temp;
                    } 
                }

                processes[process_count] = current_process;
                process_count++;
            }
            fclose(process_info);
        }
    }
    closedir(proc_dir);

    qsort(processes, process_count, sizeof(struct Process), compare_memory);

    for (size_t i = 0; i < process_count; i++) {
        printf("Name: %s | PID: %d | Memory: %d kB\n",
           processes[i].name,
           processes[i].pid,
           processes[i].memory_kb);
    }

    // top 20 memory

    printf("TOP 20 PROCESSES\n");

    for (size_t i = 0; i < 20; i++) {
        printf("Name: %s | PID: %d | Memory: %d kB\n",
           processes[i].name,
           processes[i].pid,
           processes[i].memory_kb);
    }

    free(processes);
    return 0;
}