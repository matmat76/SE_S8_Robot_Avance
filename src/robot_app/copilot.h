#ifndef COPILOT_H
#define COPILOT_H

#include <stdbool.h>
#include "pilot.h"

/*Allow to describe the status of the full movement, if all of move_t[] is done or not*/
typedef enum{
    PATH_COMPLETE,
    PATH_MOVING, 
    PATH_ERROR
}path_status_t;


bool copilot_is_path_complete();
void copilot_start_path();
void copilot_set_Path(move_t* a_path, int num_moves);
path_status_t copilot_stop_at_step_completion();
move_t* copilot_get_path();
move_t copilot_get_step(int index);
void copilot_set_number_step(int num_steps);
int copilot_get_number_step();
int copilot_destroy_path();
void copilot_add_step(move_t a_move);
int getX();
int getY();
bool is_obstacle(int x, int y);
void update_angle(move_t move);
void update_position(move_t move, int encoder_position);


#endif // COPILOT_H