#include <stdio.h>     
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>   
#include <string.h>
#include <unistd.h>
#include <pthread.h>


#define MAXPENDING 5
#define GARDEN_X 3
#define GARDEN_Y 3
#define OBSTACLE_COUNT 3
#define MOVE_TIME 1
#define CLEAR 0
#define OBSTACLE -2
#define WORKED -1

pthread_mutex_t mutex;
int garden[GARDEN_X][GARDEN_Y];

typedef struct thread_args {
    int socket;
    int number;
} thread_args;

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

void handleClient(int socket, int number) {
    int recv_buffer[4];
    int send_buffer[4];
    int x, y;
    for(;;) {
        recv(socket, recv_buffer, sizeof(recv_buffer), 0);
        if (recv_buffer[0] == -1) {
            printf("Disconected\n");
            break;
        }
        pthread_mutex_lock(&mutex);
        if (recv_buffer[0] == 1) {
            x = recv_buffer[1];
            y = recv_buffer[2];
            if (garden[x][y] > 0) {
                send_buffer[0] = 1;
            } else if (garden[x][y] == CLEAR) {
                send_buffer[0] = 2;
                garden[x][y] = number;
            } else {
                send_buffer[0] = 3;
            }
        } else if (recv_buffer[0] == 2) {
            x = recv_buffer[1];
            y = recv_buffer[2];
            garden[x][y] = WORKED;
        }
        pthread_mutex_unlock(&mutex);
        send(socket, send_buffer, sizeof(send_buffer), 0);
    }
    close(socket);
}

void *clientThread(void *args) {
    int server_socket;
    int client_socket;
    int client_length;
    struct sockaddr_in client_addr;
    pthread_detach(pthread_self());
    server_socket = ((thread_args*)args)->socket;
    int number = ((thread_args*)args)->number;
    free(args);
    listen(server_socket, MAXPENDING);
    for (;;) {
        client_length = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_length);
        printf("New connection from %s\n", inet_ntoa(client_addr.sin_addr));
        handleClient(client_socket, number);
    }
}

int createSocket(unsigned short server_port) {
    int server_socket;
    struct sockaddr_in server_addr;

    if ((server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) DieWithError("socket() failed");
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;              
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    server_addr.sin_port = htons(server_port);

    if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) DieWithError("bind() failed");
    printf("Open socket on %s:%d\n", inet_ntoa(server_addr.sin_addr), server_port);
    return server_socket;
}

int main(int argc, char *argv[])
{
    unsigned short client_1_port;
    unsigned short client_2_port;
    int server_1_socket;
    int server_2_socket;
    pthread_t thread1;
    pthread_t thread2;
    pthread_mutex_init(&mutex, NULL);
    if (argc != 3)
    {
        fprintf(stderr, "Usage:  %s <Port for 1st client> <Port for 2nd client>\n", argv[0]);
        exit(1);
    }

    for (int i = 0; i < GARDEN_X; i++) {
        for (int j = 0; j < GARDEN_Y; j++) {
            garden[i][j] = CLEAR;
        }
    }

    for (int i = 0; i < OBSTACLE_COUNT; i++) {
        garden[rand() % GARDEN_X][rand() % GARDEN_Y] = OBSTACLE;
    }

    garden[0][0] = 1;
    garden[GARDEN_X - 1][GARDEN_Y - 1] = 2;


    for (int i = 0; i < GARDEN_X; i++) {
        for (int j = 0; j < GARDEN_Y; j++) {
            printf("[%d] ",garden[i][j]);
        }
        printf("\n");
    }

    client_1_port = atoi(argv[1]);
    client_2_port = atoi(argv[2]);

    server_1_socket = createSocket(client_1_port);
    server_2_socket = createSocket(client_2_port);

    thread_args *args_1 = (thread_args*) malloc(sizeof(thread_args));
    args_1->socket = server_1_socket;
    args_1->number = 1;
    if (pthread_create(&thread1, NULL, clientThread, (void*) args_1) != 0) DieWithError("pthread_create() failed");

    thread_args *args_2 = (thread_args*) malloc(sizeof(thread_args));
    args_2->socket = server_2_socket;
    args_2->number = 2;
    if (pthread_create(&thread1, NULL, clientThread, (void*) args_2) != 0) DieWithError("pthread_create() failed");

    for (;;) {
        for (int i = 0; i < GARDEN_X; i++) {
            for (int j = 0; j < GARDEN_Y; j++) {
                printf("[%d] ",garden[i][j]);
            }
            printf("\n");
        }
        printf("----------\n");
        sleep(1);
    }
    pthread_mutex_destroy(&mutex);
    return 0;
}