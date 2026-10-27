#include <stdio.h>
#include <string.h>

// Start of the source c code for system_monitor application
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

    while (fgets(line, sizeof(line), sys_info) != NULL) {
        if (strncmp(line, "Pid:", 4) == 0) {
            printf("%s", line);
        }

        if (strncmp(line, "VmRSS:", 6) == 0) {
            printf("%s", line);
        }
    }

    fclose(sys_info);

    return 0;
}