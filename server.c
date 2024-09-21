#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <dirent.h>


#define PORT 5555
#define MAX_PROCESSES 1024
#define BUFFER_SIZE 1024

typedef struct {
    char name[256];
    pid_t pid;
    unsigned long uTime;
    unsigned long sTime;
    unsigned long totalTime;
} ProcessInfo;

int compareProcesses(const void *a, const void *b) {
    return ((ProcessInfo *)b)->totalTime - ((ProcessInfo *)a)->totalTime;
}

void getTopCpuProcesses(ProcessInfo *processes, int *count) {
    DIR *proc_dir = opendir("/proc");
    struct dirent *entry;
    *count = 0;

    while ((entry = readdir(proc_dir)) != NULL) {
        if (entry->d_type == DT_DIR && atoi(entry->d_name) > 0) {
            const pid_t pid = atoi(entry->d_name);
            char stat_file[256];
            snprintf(stat_file, sizeof(stat_file), "/proc/%d/stat", pid);
            FILE *file = fopen(stat_file, "r")  ;

            if (file) {
                ProcessInfo process;
                fscanf(file, "%d %s %*c %*d %*d %*d %*d %*d %*d %*d %*d %lu %lu",
                       &process.pid, process.name, &process.uTime, &process.sTime);
                process.totalTime = process.uTime + process.sTime;
                processes[(*count)++] = process;
                fclose(file);
            }
        }
    }
    closedir(proc_dir);

    // Sort processes by total CPU time
    qsort(processes, *count, sizeof(ProcessInfo), compareProcesses);
}

int main(void) {
    int sockfd;
    struct sockaddr_in serverAddr;

    int newSocket;
    struct sockaddr_in newClientAddr;

    socklen_t addrLength;

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error opening socket\n");
        exit(EXIT_FAILURE);
    }
    printf("Server Socket created successfully....\n");

    // cleaning the serverAddr and then assigning values
    memset(&serverAddr,'\0',sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //hardcoding it rn

    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr))<0) {
        perror("Error on binding\n");
        exit(EXIT_FAILURE);
    }
    printf("Server binding successfully....\n Port: %d\n", PORT);
    while(1) {
        char response[BUFFER_SIZE] = {0};
        if (listen(sockfd, 5) < 0) {
            perror("Error on listening\n");
            exit(EXIT_FAILURE);
        }
        printf("Server listening....\n Port: %d\n", PORT);

        if ((newSocket = accept(sockfd, (struct sockaddr *)&newClientAddr, &addrLength))<0) {
            perror("Error on accept\n");
            exit(EXIT_FAILURE);
        }

        ProcessInfo processes[MAX_PROCESSES];
        int count;
        getTopCpuProcesses(processes, &count);


        for (int i = 0; i < 2 && i < count; i++) {
            snprintf(response + strlen(response), sizeof(response) - strlen(response),
                     "Name: %s, PID: %d, User Time: %lu, Kernel Time: %lu\n",
                     processes[i].name, processes[i].pid, processes[i].uTime, processes[i].sTime);
        }
        if (send(newSocket, response, strlen(response), 0)<0) {
            perror("Error on send\n");
            exit(EXIT_FAILURE);
        }
        printf("Server sending message...\n");

    }

    return 0;
}
