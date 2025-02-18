#ifndef PILOT_H
#define PILOT_H

#include "robot.h"
#include <time.h>
/*Give the status that the robot is doing*/
/*GIve the general status of the robot*/
typedef enum{
    MOVE_FORWARD,
    MOVE_ROTATION,
    MOVE_DONE,
    MOVE_OBSTACLE_FORWARD
}move_status_t;

typedef enum{
    DIRECTION_UP,
    DIRECTION_RIGHT,
    DIRECTION_DOWN,
    DIRECTION_LEFT
}direction_t; 
/*Give the order to the robot*/
typedef enum{
    FORWARD,
    ROTATION
}move_type_t;

/*Gives the directions for the robot */
typedef enum{
    RIGHT,
    LEFT,   
    U_TURN //Turn over
}angle_t; 

typedef union{
    int distance; 
    angle_t angle;
}range_t;

/*Allow to parameter the actions with 3 parameters : move_status_t, direction, speed*/
typedef struct{
    move_type_t move_type; 
    range_t range;
    speed_pct_t speed; 
}move_t;

typedef struct{
    clock_t start_time; 
    int duration;
}move_timing_t;


move_status_t pilot_get_status();
move_type_t pilot_get_move_type();
move_status_t pilot_stop_at_target();
void pilot_start_move(move_t a_move);

#endif // PILOT_H 