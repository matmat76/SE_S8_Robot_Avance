#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "./sock.h"

#include "../../../utils.h"

static int sockfd;

void sock_init(void){
    // Initialization code if needed
}

void sock_connect(void){
    struct sockaddr_in server_addr;
    struct hostent *he;

    if ((he = gethostbyname(SERVER_ADDRESS)) == NULL) {
        perror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr = *((struct in_addr *)he->h_addr);
    memset(&(server_addr.sin_zero), '\0', 8);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect");
        close(sockfd);
        exit(1);
    }

    printf("Connected to server at %s:%d\n", SERVER_ADDRESS, SERVER_PORT);
}

void sock_close(void){
    close(sockfd);
}


void sock_send_detailed_order(move_t *steps, int steps_number) {
    // Prepare the data to be sent
    sock_data_t send_data;
    send_data.num_moves = steps_number;
    send_data.moves = steps;

    // Send the number of moves
    if (send(sockfd, &send_data.num_moves, sizeof(int), 0) == -1) {
        perror("send");
        close(sockfd);
        exit(1);
    }

    // Send the moves
    if (send(sockfd, send_data.moves, sizeof(move_t) * send_data.num_moves, 0) == -1) {
        perror("send");
        close(sockfd);
        exit(1);
    }

    printf("Data sent to server\n");

    // Clean up
    close(sockfd);
}