#ifndef TEXT_PROMPTS_H
#define TEXT_PROMPTS_H

// Menu options
typedef enum {
    CMD_CREATE_PATH,
    CMD_ADD_STEP,
    CMD_DESTROY_PATH,
    CMD_START_PATH,
    CMD_QUIT,
    NB_COMMAND
  } menu_command;
  
  typedef enum {
    UI_WELCOME = 0,
    UI_ASK_CHOICE,
    UI_ASK_SIZE_PATH,
    UI_ASK_STEP_NUMBER,
    UI_ASK_SPEED,
    UI_ASK_TYPE_MOVE,
    UI_ASK_VALUE_FORWARD,
    UI_ASK_VALUE_TURN,
    NB_PROMPT_MESSAGE
  } id_message_prompt;

    void print_prompt_message(id_message_prompt id);
    void print_success_message(menu_command cmd);
    void print_failure_message(menu_command cmd);
    void print_menu();
  
  
#endif // TEXT_PROMPTS_H