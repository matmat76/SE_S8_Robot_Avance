#ifndef SOCK_H
#define SOCK_H

#include "../../../../common/data-type.h"

void sock_init(void);

void sock_connect(void);

void sock_close(void);

// void sock_send_order(order_type_t order);

void sock_send_detailed_order(move_t *steps, int steps_number);



#endif // SOCK_H