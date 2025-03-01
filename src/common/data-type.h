#ifndef DATA_TYPE_H
#define DATA_TYPE_H

// Robot data types

typedef int speed_pct_t;

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


// Socket data types

typedef enum {
    COPILOT_SET_PATH,
    COPILOT_GET_PATH,
    COPILOT_DESTROY_PATH,
    COPILOT_START_PATH,
    COPILOT_STOP_AT_STEP_COMPLETION,
    COPILOT_IS_PATH_COMPLETE
} order_type_t;

typedef struct {
    move_t* path;
    int path_size;
} order_details_t;

typedef struct{
    order_type_t order_type;
    order_details_t order_details;
}sock_data_t;

#endif // DATA_TYPE_H