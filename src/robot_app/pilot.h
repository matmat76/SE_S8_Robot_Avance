#ifndef PILOT_H
#define PILOT_H

#include "robot.h"
#include <time.h>
#include "../common/data-type.h"


move_status_t pilot_get_status();
move_type_t pilot_get_move_type();
move_status_t pilot_stop_at_target();
void pilot_start_move(move_t a_move);

#endif // PILOT_H 