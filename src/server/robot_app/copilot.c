#include "pilot.h"
#include "copilot.h"
#include "stdbool.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define GRID_SIZE 11
#define UNIT_DISTANCE 200

static int current_step;
int path_step;
path_status_t path_status;
static move_t *path;
static int distance_covered =0;
static int x = 0; 
static int  y=0;

direction_t current_angle = DIRECTION_UP;

/*Getter for x and y */
int getX(){
    return x;
}
int getY(){
    return y;
}

int grid[GRID_SIZE][GRID_SIZE] = {
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

bool is_obstacle(int x, int y) {
    // Transformation des coordonnées réelles vers les indices du tableau
    int grid_x = x + 5; // Décalage pour rendre x entre 0 et 10
    int grid_y = y + 5; // Décalage pour rendre y entre 0 et 10
    
    // Vérifier si la position est valide dans le tableau
    if (grid_x >= -5 && grid_x < 5 && grid_y >= -5 && grid_y < 5) {
        return grid[grid_x][grid_y] == 1;  // Si l'obstacle est présent sur cette case
    }
    return false; // Si on sort du quadrillage, on considère qu'il n'y a pas d'obstacle
}
void copilot_set_Path(move_t* a_path, int num_moves){
    if (path != NULL){
        free(path);
    }
    path = (move_t*)calloc(num_moves,sizeof(move_t));
    path = memcpy(path,a_path,num_moves*sizeof(move_t));
    path_step = num_moves;
}

void update_angle(move_t move){
    if((move.move_type)== ROTATION){
        switch(move.range.angle){
            case RIGHT:
                current_angle = (current_angle + 1) % 4;
                fprintf(stdout,"Nouvelle direction après rotation vers la droite %d\n",current_angle);
                break;
            case LEFT:
                current_angle = (current_angle + 3) % 4;
                fprintf(stdout,"Nouvelle direction après rotation vers la gauche %d\n",current_angle);
                break;
            case U_TURN:
                current_angle = (current_angle + 2) % 4;
                fprintf(stdout,"Nouvelle direction après demi-tour %d\n",current_angle);
                break;
            default:
                fprintf(stdout,"No changement of direction\n");
                break;
        }
    }
}

void update_position(move_t move, int encoder_position){
    if(move.move_type == FORWARD){
        /*Add the value of the new_distance with the robot encoder*/
        int new_distance = encoder_position;
        fprintf(stdout,"Distance parcourue : %d/%d\n",new_distance,UNIT_DISTANCE);

        if(new_distance >= UNIT_DISTANCE){
            switch(current_angle){
                case DIRECTION_UP: 
                    fprintf(stdout,"Déplacement vers le haut, nouvelle position : %d",y); 
                    if(!is_obstacle(x,y+1)){
                        y++; 
                        fprintf(stdout,"Déplacement vers le haut, nouvelle position : %d",y); 
                    }else{
                        fprintf(stdout,"Obstacle détecté en (%d, %d)\n", x, y + 1);
                        return; 
                    }break; 
                case DIRECTION_RIGHT :
                    fprintf(stdout,"Déplacement vers la droite, nouvelle position : %d",x);
                    if(!is_obstacle(x+1,y)){
                        x++; 
                        fprintf(stdout,"Déplacement vers la droite, nouvelle position : %d",x);
                    }else{
                        fprintf(stdout,"Obstacle détecté en (%d, %d)\n", x + 1, y);
                        return;
                    }break;
                case DIRECTION_DOWN:
                    fprintf(stdout,"Déplacement vers le bas, nouvelle position : %d",y);
                    if(!is_obstacle(x,y-1)){
                        y--; 
                        fprintf(stdout,"Déplacement vers le bas, nouvelle position : %d",y);
                    }else{
                        fprintf(stdout,"Obstacle détecté en (%d, %d)\n", x, y - 1);
                        return;
                    }break;
                case DIRECTION_LEFT:
                    fprintf(stdout,"Déplacement vers la gauche, nouvelle position : %d",x);
                    if(!is_obstacle(x-1,y)){
                        x--; 
                        fprintf(stdout,"Déplacement vers la gauche, nouvelle position : %d",x);
                    }else{
                        fprintf(stdout,"Obstacle détecté en (%d, %d)\n", x - 1, y);
                        return;
                    }break;
            }new_distance -=UNIT_DISTANCE;
        }else{
            distance_covered = new_distance;
        }
    }
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
    current_angle = DIRECTION_UP;
    distance_covered = 0; 
    x = 0; 
    y = 0; 
    for(int i =0; i<copilot_get_number_step(); i++){
        path_status = PATH_MOVING; 
        fprintf(stdout,"Entering pilot start move\n");
        fprintf(stdout,"Step %d, type = %d, speed = %d\n",
                current_step,
                path[current_step].move_type,
                path[current_step].speed);
        pilot_start_move(path[current_step]);
        fprintf(stdout,"%d move started\n",i);

    }
     

}

path_status_t copilot_stop_at_step_completion(){
    fprintf(stdout,"Checking move completion ...\n"); 
    move_status_t current_status = pilot_stop_at_target();
    fprintf(stdout,"Current move status : %d\n",current_status);
    
    if(path[current_step].move_type == FORWARD){
        int current_pos = robot_get_wheel_position(LEFT_WHEEL);
        update_position(path[current_step],current_pos);
    }

    if(current_status == MOVE_DONE){
        update_angle(path[current_step]);

        fprintf(stdout,"Move completed, moving to the next step\n");
        current_step++;
        if(current_step == path_step){
            path_status = PATH_COMPLETE;
            fprintf(stdout,"Path completed at position x : %d, y : %d\n",x,y);
        }else{
            pilot_start_move(path[current_step]);
            fprintf(stdout,"Starting step %d with angle %d\n",current_step,current_angle);
            fprintf(stdout,"Robot at position(%d, %d)\n",x,y); 
        }
    }
    return path_status;
}

/*Return the true or false if the variable path_status is PATH_COMPLETE
In other words : if the whole movements are */
bool copilot_is_path_complete(){
    return path_status == PATH_COMPLETE; 
}