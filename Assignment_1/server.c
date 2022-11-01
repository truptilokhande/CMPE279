// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <pwd.h>
#include <string.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    const char *nobodyUser = "nobody";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    printf("Forking a child process.\n");
    pid_t processId = fork();
    if (processId < 0)
    {
        printf("ERROR in forking child\n");
        return -1;
    }
    else if (processId == 0)
    {
        printf("Forked! Inside Child Process\n");
        struct passwd *pw = getpwnam(nobodyUser);
        if (pw == NULL)
        {
            printf("ERROR in finding UID for name %s\n", nobodyUser);
            return -1;
        }
        int dropStatus = setuid(pw->pw_uid);

        if (dropStatus < 0)
        {
            printf("ERROR in dropping privileges | dropStatus = %d\n", dropStatus);
            return -1;
        }
        printf("Child process dropped privileges\n");
        valread = read(new_socket, buffer, 1024);
        printf("MESSAGE RECEIVED: %s\n", buffer);
        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent\n");
        return 0;
    }
    else
    {
        wait(NULL);
        printf("Child process finished.\nParent Terminating\n");
    }
}