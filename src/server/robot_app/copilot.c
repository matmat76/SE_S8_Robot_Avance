#include "pilot.h"
#include "copilot.h"
#include "stdbool.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define GRID_SIZE 11
#define UNIT_DISTANCE 200

Pilot *pilot;

static int current_step;
int path_step;
path_status_t path_status;
static move_t *path;
static int distance_covered =0;
static int x = 0; 
static int  y=0;

direction_t current_angle = DIRECTION_UP;




void copilot_init(void){
    pilot = Pilot_new();
}


void copilot_set_Path(move_t* a_path, int num_moves){
    if (path != NULL){
        free(path);
    }
    path = (move_t*)calloc(num_moves,sizeof(move_t));
    path = memcpy(path,a_path,num_moves*sizeof(move_t));
    path_step = num_moves;
}

move_t* copilot_get_path(){
    return path;
}
move_t copilot_get_step(int index){
    return path[index];
}

void copilot_set_number_step(int num_steps){
    path_step = num_steps;
}

int copilot_get_number_step(){
    return path_step;
}

int copilot_destroy_path(){
    if(path != NULL){
        free(path); 
        path = NULL;
        return 0; 
    }else{
        path_status = PATH_ERROR; 
        return -1;
    }
}
void copilot_add_step(move_t a_move){
    if(path == NULL){
        return; 
    }
    path[current_step] = a_move;
    current_step++;
}

void copilot_start_path(){
    if(path == NULL){
        path_status = PATH_ERROR;
        return;
    }
    fprintf(stdout,"Starting path\n");
    current_step = 0;
    
    for(int i =0; i<copilot_get_number_step(); i++){
        path_status = PATH_MOVING; 
        fprintf(stdout,"Entering pilot start move\n");
        fprintf(stdout,"Step %d, type = %d, speed = %d\n",
                current_step,
                path[current_step].move_type,
                path[current_step].speed);
        Pilot_set_move(pilot,path[current_step]);
        fprintf(stdout,"%d move started\n",current_step);

    }
     

}

path_status_t copilot_stop_at_step_completion(){
    fprintf(stdout,"Checking move completion ...\n"); 
    move_status_t current_status = Pilot_wait_move(pilot);
    fprintf(stdout,"Current move status : %d\n",current_status);

    fprintf(stdout,"Move completed, moving to the next step\n");
    current_step++;
    if(current_step == path_step){
        path_status = PATH_COMPLETE;
        fprintf(stdout,"Path completed at position x : %d, y : %d\n",x,y);
    }else{
        Pilot_set_move(pilot,path[current_step]);
        fprintf(stdout,"Starting step %d with angle %d\n",current_step,current_angle);
        fprintf(stdout,"Robot at position(%d, %d)\n",x,y); 
    }
    return path_status;
}

/*Return the true or false if the variable path_status is PATH_COMPLETE
In other words : if the whole movements are */
bool copilot_is_path_complete(){
    return path_status == PATH_COMPLETE; 
}