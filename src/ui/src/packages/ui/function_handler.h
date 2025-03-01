#ifndef FUNCTION_HANDLER_H
#define FUNCTION_HANDLER_H

#include "text_prompts.h"

/* Function prototypes */
int handle_user_prompt_int(id_message_prompt id, int min, int max);
void handle_create_path();
void handle_add_step();
void handle_destroy_path();
int handle_start_path();

#endif // FUNCTION_HANDLER_H