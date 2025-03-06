#ifndef PILOT_H
#define PILOT_H

#include "robot.h"
#include <time.h>
#include "../../common/data-type.h"

typedef struct Pilot_t Pilot;

Pilot *Pilot_new(void);
void Pilot_destroy(Pilot* pilot);

move_status_t Pilot_get_status(Pilot* this);
move_type_t ¨Pilot_get_move_type(Pilot* this);

void Pilot_set_move(Pilot* this, move_t a_move);
move_status_t Pilot_wait_move(Pilot* this);


#endif // PILOT_H 