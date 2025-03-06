/**
 * main program with pilot and robot modules
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <errno.h>
 #include <string.h>
 #include <netdb.h>
 #include <sys/types.h>
 #include <netinet/in.h>
 #include <sys/socket.h>
 #include "../common/data-type.h"
 #include <signal.h>
 #include <arpa/inet.h>  // Ajoutez cette ligne pour inet_ntoa

#include "robot_app/robot.h"
#include "robot_app/copilot.h"
/**
 * @mainpage Robot application mission 2.
 * This projects aims to move a robot along a predefined trajectory.
 */

/**
 * @enum process_state_t
 * @brief indicate the process state management for the application.
 */
typedef enum {
  STOPPED = 0, /**< Process is stopped */
  ALIVE,      /**< Process is running */
} process_state_t;

/** @brief Waiting time between two encoder's scans (in microseconds).
 * Adapt according to your cpu and simulator.
 */
#define DELAY 100000

/** @brief Max scanning's attempt to check if a move is finished.
 * Adapt according your move
 */
#define ENCODERS_SCAN_NB 1000

#define STEPS_NUMBER 6 /**< number of steps (or moves) in the path */

#define PORT_NUM 3490
#define BACKLOG 10

// declaration of private functions
static void app_loop(void);
static void handle_client_data(int client_socket); 
static void setup_serveur(void);

/**
 * @brief Global variable used for program clean exit
 */
static process_state_t running = ALIVE;

/**
 * @brief Function for CTRL+C signal management
 */
static void sigint_handler(int dummy) { running = STOPPED; }

int main(void)
{
  /* open the robot simulator */
  printf("**** Version démo RM2 **** \n");
  printf("**** par JDL **** \n");
  printf("Ctrl+C pour quitter\n");
  fflush(stdout);
  /*Initilize robot first*/
  if(robot_start()!=0){
    fprintf(stderr,"Fatal error : Robot initialization failed\n");
    return EXIT_FAILURE;
  }
  /* Ctrl+C to stop the program. */
  signal(SIGINT, sigint_handler);

  /* start the robot simulator */
  app_loop();
  /* close the robot simulator */
  robot_close();
  return EXIT_SUCCESS;
}

/**
 * @brief Main loop for the application.
 * Send commands to the pilot and display robot's status with a specific period.
 */

static void app_loop()
{
  copilot_init();
  setup_serveur();
}


static void handle_client_data(int client_socket){
  sock_data_t receive_data;
  ssize_t bytes_received; 

  bytes_received = recv(client_socket, &receive_data.num_moves, sizeof(int), 0);
  if(bytes_received == -1){
    perror("error lors de la récupération du nombre de mouvements\n");
    return;
  }

  receive_data.moves = malloc(sizeof(move_t)*receive_data.num_moves);

  if(receive_data.moves == NULL){
    perror("error lors de l'allocation de la mémoire pour les mouvements\n");
    return;
  }


  bytes_received = recv(client_socket, receive_data.moves, sizeof(move_t)*receive_data.num_moves, 0);
  if(bytes_received == -1){
    perror("error lors de la récupération des mouvements\n");
    return;
  }

  if(bytes_received <=0){
    free(receive_data.moves);
    perror("Error de la récupération des mouvements\n");
    return;
  }

  copilot_set_Path(receive_data.moves, receive_data.num_moves);
  copilot_start_path();
  while(!copilot_is_path_complete()){
    copilot_stop_at_step_completion();
  }
  free(receive_data.moves);

}

static void setup_serveur(void){
  int sockfd, new_fd;
  struct sockaddr_in my_addr;
  struct sockaddr_in their_addr;
  socklen_t sin_size;
  int yes = 1;

  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    perror("socket");
    exit(1);
  }

  if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
    perror("setsockopt");
    exit(1);
  }

  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(PORT_NUM);
  my_addr.sin_addr.s_addr = INADDR_ANY;
  memset(&(my_addr.sin_zero), '\0', 8);

  if(bind(sockfd,(struct sockaddr*)&my_addr, sizeof(struct sockaddr)) == -1){
    perror("bind");
    exit(1);
  }

  if(listen(sockfd,BACKLOG)==-1){
    perror("listen");
    exit(1);
  }

  fprintf(stderr, "server: waiting for connections...\n");
  while(running){
    sin_size = sizeof(struct sockaddr_in);
    if((new_fd = accept(sockfd, (struct sockaddr*)&their_addr, &sin_size)) == -1){
      perror("accept");
      continue;
    }
    printf("server: got connection from %s\n", inet_ntoa(their_addr.sin_addr));
    handle_client_data(new_fd);
    close(new_fd);
  }
  
  // Close the server socket when done
  close(sockfd);
}