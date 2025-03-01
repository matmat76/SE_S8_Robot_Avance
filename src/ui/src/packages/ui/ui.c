#include "ui.h"
#include "function_handler.h"
#include "text_prompts.h"
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    S_IDLE = 0,
    S_RUNNING,
    S_EXIT,
    NB_STATE
} state_t;

typedef enum {
    E_CREATE_PATH = 0,
    E_ADD_STEP,
    E_DESTROY_PATH,
    E_START_PATH,
    E_QUIT,
    NB_EVENT
} event_t;

typedef enum {
    A_CREATE_PATH = 0,
    A_ADD_STEP,
    A_DESTROY_PATH,
    A_START_PATH,
    A_QUIT,
    A_NOP
} action_enum_t;

typedef struct {
    state_t state;
} ui_t;

typedef struct {
    state_t destination_state;
    action_enum_t transition_action;
} transition_t;

typedef void (*action_t)(ui_t*);

static void action_create_path(ui_t* ui) { handle_create_path(); }
static void action_add_step(ui_t* ui) { handle_add_step(); }
static void action_destroy_path(ui_t* ui) { handle_destroy_path(); }
static void action_start_path(ui_t* ui) { if (handle_start_path() == EXIT_SUCCESS) ui->state = S_EXIT; }
static void action_quit(ui_t* ui) { print_success_message(CMD_QUIT); ui->state = S_EXIT; }
static void action_nop(ui_t* ui) {}

static const action_t actions[] = {
    [A_CREATE_PATH] = action_create_path,
    [A_ADD_STEP] = action_add_step,
    [A_DESTROY_PATH] = action_destroy_path,
    [A_START_PATH] = action_start_path,
    [A_QUIT] = action_quit,
    [A_NOP] = action_nop
};

static const transition_t transition_table[NB_STATE][NB_EVENT] = {
    [S_IDLE][E_CREATE_PATH] = {S_RUNNING, A_CREATE_PATH},
    [S_IDLE][E_ADD_STEP] = {S_RUNNING, A_ADD_STEP},
    [S_IDLE][E_DESTROY_PATH] = {S_RUNNING, A_DESTROY_PATH},
    [S_IDLE][E_START_PATH] = {S_RUNNING, A_START_PATH},
    [S_IDLE][E_QUIT] = {S_EXIT, A_QUIT},
    [S_RUNNING][E_CREATE_PATH] = {S_RUNNING, A_CREATE_PATH},
    [S_RUNNING][E_ADD_STEP] = {S_RUNNING, A_ADD_STEP},
    [S_RUNNING][E_DESTROY_PATH] = {S_RUNNING, A_DESTROY_PATH},
    [S_RUNNING][E_START_PATH] = {S_RUNNING, A_START_PATH},
    [S_RUNNING][E_QUIT] = {S_EXIT, A_QUIT},
    [S_EXIT][E_CREATE_PATH] = {S_EXIT, A_NOP},
    [S_EXIT][E_ADD_STEP] = {S_EXIT, A_NOP},
    [S_EXIT][E_DESTROY_PATH] = {S_EXIT, A_NOP},
    [S_EXIT][E_START_PATH] = {S_EXIT, A_NOP},
    [S_EXIT][E_QUIT] = {S_EXIT, A_NOP}
};

static void run(ui_t* ui, event_t event) {
    actions[transition_table[ui->state][event].transition_action](ui);
    ui->state = transition_table[ui->state][event].destination_state;
}

extern int ui_start() {
    ui_t ui = {S_IDLE};
    int choice;

    while (ui.state != S_EXIT) {
        print_menu();
        choice = handle_user_prompt_int(UI_ASK_CHOICE, 0, (NB_COMMAND - 1));
        printf("Value of the user's choice : %d\n", choice);
        run(&ui, (event_t)choice);
    }

    return EXIT_SUCCESS;
}
