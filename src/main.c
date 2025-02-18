/**
 * main program with pilot and robot modules
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "robot_app/robot.h"
#include "robot_app/copilot.h"
#include "ui/ui.h"
#include "utils.h"

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


// declaration of private functions
static void app_loop(void);

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
  
  /*Start running the IHM*/
  ui_start();
  /*Have to define the random path from copilot*/
  while (running){

    /* Encoders polling */
    for (int i = 0; i < ENCODERS_SCAN_NB; i++){
      usleep(DELAY);
    }
    fprintf(stdout,"Actual position x = %d, y = %d\n",getX(),getY());
  
  }
}
