#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>

#include "robot.h"
#include "pilot.h"
#include "stdbool.h"
#define DISTANCE_FORWARD 200 // distance to move forward 
#define DISTANCE_TURN 232 // distance of the encoder to make 90°

/* === States and event */

typedef enum{
    S_DEAD = 0,
    S_IDLE,
    S_MOVE_IN_PROGRESS,
    NB_STATE,
}state_t;

typedef enum{
    E_KILL = 0,
    E_NEW_MOVE,
    E_WAIT_MOVE,
    NB_EVENT,
}event_t;

typedef enum
{
    A_NOP = 0,
	A_KILL,
	A_START_MOVE,
	A_WAIT_MOVE,
    NB_ACTION,
}action_enum_t;

/* === Object descriptor === */
struct Pilot_t{
    state_t state;
    move_status_t robot_moving;
    int target_pos; 
    wheel_t reference_wheel; 
};

/* === Corelation table === */
typedef struct{
	state_t destination_state;
	action_enum_t transition_action;
}transition_t;
typedef void (*action_t)(Pilot*, ...);

static void start_move(Pilot* this, va_list args);
static void wait_end_move(Pilot* this);

static void empty_action(Pilot* this, ...){
    fprintf(stdout,"Pilot >>> Empty action\n");
}

static const action_t action_table[NB_ACTION] = 
{
    [A_NOP] = empty_action,
    [A_KILL] = Pilot_destroy,
    [A_START_MOVE] = start_move,
    [A_WAIT_MOVE] = wait_end_move,
};

static const transition_t transition_table[NB_STATE][NB_EVENT] = 
{
	[S_IDLE][E_KILL] = {S_DEAD, A_KILL},
    [S_MOVE_IN_PROGRESS][E_KILL] = {S_DEAD, A_KILL},
    [S_IDLE][E_NEW_MOVE] = {S_MOVE_IN_PROGRESS, A_START_MOVE},
    [S_MOVE_IN_PROGRESS][E_NEW_MOVE] = {S_MOVE_IN_PROGRESS, A_START_MOVE},
    [S_MOVE_IN_PROGRESS][E_WAIT_MOVE] = {S_IDLE, A_WAIT_MOVE},
};

/* ====== Functions ====== */

/* --- Object related functions --- */
extern Pilot *Pilot_new(void){
    Pilot* pilot = (Pilot*)malloc(sizeof(Pilot));

    if(pilot == NULL){
        perror("Error of allocation of memory for the pilot\n");
        return NULL;
    }

    //Set default value 
    pilot->state = S_IDLE;
    pilot->robot_moving = MOVE_DONE;
    pilot->target_pos = 0;
    pilot->reference_wheel = LEFT_WHEEL;
    return pilot;
}

extern void Pilot_destroy(Pilot* this){
    free(this);
}

/* --- Getter --- */
extern move_status_t Pilot_get_status(Pilot* this){
    return this->robot_moving;
}

extern move_type_t pilot_get_move_type(Pilot* this){
    move_type_t move_type;
    if(this->robot_moving == MOVE_FORWARD){
        move_type = FORWARD; 
    }else{
        move_type = ROTATION; 
    }
    return move_type;
}

/* --- Internal function --- */
static void run_action(Pilot* this, event_t event, ...){
    fprintf(stdout,"Pilot >>> Run action\n");
    va_list args;
    va_start(args, event);
    int action_index = transition_table[this->state][event].transition_action;
    fprintf(stdout,"Pilot >>> Action index : %d\n",action_index);
    action_t action = action_table[action_index];
    if(action != NULL){
        action(this, args);
        this->state = transition_table[this->state][event].destination_state;
    }
    va_end(args);
}

static void start_move(Pilot* this, va_list args){
    fprintf(stdout,"Pilot >>> Start Move\n");
    static int compteur = 0;

    move_t a_move = va_arg(args, move_t);

    if(this->robot_moving == MOVE_DONE){
        fprintf(stdout,"Pilot.c : Starting move %d\n",compteur++ +1);

        /*Starting timer*/
        switch(a_move.move_type){
            case FORWARD:
                /*Calcul of the time with the distance*/
                this->reference_wheel = LEFT_WHEEL;
                this->target_pos = robot_get_wheel_position(this->reference_wheel)+ a_move.range.distance*DISTANCE_FORWARD;
                robot_set_speed(a_move.speed,a_move.speed);
                this->robot_moving = MOVE_FORWARD;
                compteur ++;
                fprintf(stdout,"Pilot.c : Forward %d \n",compteur);
                break;
            case ROTATION:
            /*We define the target for the rotation in advance*/      
                switch(a_move.range.angle){
                    case RIGHT:
                        this->reference_wheel = LEFT_WHEEL;
                        this->target_pos = robot_get_wheel_position(this->reference_wheel) + DISTANCE_TURN;
                        robot_set_speed(a_move.speed,-a_move.speed);
                        this->robot_moving = MOVE_ROTATION;
                        compteur ++;
                        fprintf(stdout,"Right %d\n",compteur);
                        break;
                    case LEFT:
                        this->reference_wheel = RIGHT_WHEEL;
                        this->target_pos = robot_get_wheel_position(this->reference_wheel) + DISTANCE_TURN;
                        robot_set_speed(-a_move.speed,a_move.speed);
                        this->robot_moving = MOVE_ROTATION;
                        compteur ++;
                        fprintf(stdout,"Left %d\n",compteur);
                        break;
                    case U_TURN:
                        this->reference_wheel = RIGHT_WHEEL;
                        this->target_pos = robot_get_wheel_position(this->reference_wheel) + DISTANCE_TURN*2;
                        robot_set_speed(-a_move.speed,a_move.speed);
                        this->robot_moving = MOVE_ROTATION;
                        compteur ++;
                        fprintf(stdout,"U_turn %d\n",compteur);
                        break;
                    default:
                        fprintf(stdout,"Error of action rotation\n");
                        break;
                }break;
            default:
                fprintf(stdout,"Error of action move \n");
                break;
            
        }
    }
}

static void wait_end_move(Pilot *this){
    fprintf(stdout,"Pilot stop at target\n");
    int current_pos = robot_get_wheel_position(this->reference_wheel);

    while (this->robot_moving != MOVE_DONE)
    {
        if(current_pos == -1){
            fprintf(stdout,"Error : invalid encoder"); 
            robot_set_speed(0,0);
            this->robot_moving = MOVE_DONE;
            return;
        }
        
        if(current_pos >= this->target_pos){
            robot_set_speed(0,0);
            this->robot_moving = MOVE_DONE;
            robot_reset_wheel_pos();
            fprintf(stdout,"Move done\n");
            return;
        }
    }
    return;
}

/* --- External function --- */


extern void Pilot_set_move(Pilot* this, move_t a_move){
    fprintf(stdout,"Pilot >>> set move\n");
    run_action(this, E_NEW_MOVE, a_move);
}

extern move_status_t Pilot_wait_move(Pilot* this){
    run_action(this, E_WAIT_MOVE);
    return Pilot_get_status(this);
}



/* -- OLD -- */
/*
static move_status_t robot_moving = MOVE_DONE;
static int target_pos; 
static wheel_t reference_wheel; 



void pilot_start_move(move_t a_move){
    static int compteur = 0;

    if(robot_moving == MOVE_DONE){
        fprintf(stdout,"Pilot.c : Starting move %d\n",compteur++ +1);

        //Starting timer
        switch(a_move.move_type){
            case FORWARD:
                //Calcul of the time with the distance
                reference_wheel = LEFT_WHEEL;
                target_pos = robot_get_wheel_position(reference_wheel)+ a_move.range.distance*DISTANCE_FORWARD;
                robot_set_speed(a_move.speed,a_move.speed);
                robot_moving = MOVE_FORWARD;
                compteur ++;
                fprintf(stdout,"Pilot.c : Forward %d \n",compteur);
                break;
            case ROTATION:
            //We define the target for the rotation in advance      
                switch(a_move.range.angle){
                    case RIGHT:
                        reference_wheel = LEFT_WHEEL;
                        target_pos = robot_get_wheel_position(reference_wheel) + DISTANCE_TURN;
                        robot_set_speed(a_move.speed,-a_move.speed);
                        robot_moving = MOVE_ROTATION;
                        compteur ++;
                        fprintf(stdout,"Right %d\n",compteur);
                        break;
                    case LEFT:
                        reference_wheel = RIGHT_WHEEL;
                        target_pos = robot_get_wheel_position(reference_wheel) + DISTANCE_TURN;
                        robot_set_speed(-a_move.speed,a_move.speed);
                        robot_moving = MOVE_ROTATION;
                        compteur ++;
                        fprintf(stdout,"Left %d\n",compteur);
                        break;
                    case U_TURN:
                        reference_wheel = RIGHT_WHEEL;
                        target_pos = robot_get_wheel_position(reference_wheel) + DISTANCE_TURN*2;
                        robot_set_speed(-a_move.speed,a_move.speed);
                        robot_moving = MOVE_ROTATION;
                        compteur ++;
                        fprintf(stdout,"U_turn %d\n",compteur);
                        break;
                    default:
                        fprintf(stdout,"Error of action rotation\n");
                        break;
                }break;
            default:
                fprintf(stdout,"Error of action move \n");
                break;
            
        }
    }
}


move_status_t pilot_stop_at_target(){
    fprintf(stdout,"Pilot stop at target\n");
    int current_pos = robot_get_wheel_position(reference_wheel);

    if(current_pos == -1){
        fprintf(stdout,"Error : invalid encoder"); 
        robot_set_speed(0,0);
        robot_moving = MOVE_DONE;
        return robot_moving;
    }
    
    if(current_pos >= target_pos){
        robot_set_speed(0,0);
        robot_moving = MOVE_DONE;
        robot_reset_wheel_pos();
        fprintf(stdout,"Move done\n");
    }
    return robot_moving;
}

*/