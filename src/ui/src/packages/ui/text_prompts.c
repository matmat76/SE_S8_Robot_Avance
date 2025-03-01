#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "text_prompts.h"

static const char *prompt_messages[] = {
    "\n=== User Interface ===\n",
    "Enter your choice",
    "Enter the numer of step in the path",
    "Enter the step number",
    "Enter the speed",
    "Enter the type of move (0:Forward, 1:Rotation)",
    "Enter the distance to go forward",
    "Enter the type of rotation (1:left, 2:right, 3: flip turn)"};

static const char *menu_strings[] = {
    "Create a path",            // CMD_CREATE_PATH
    "Add a step to the Path",   // CMD_ADD_STEP
    "Destroy the path",         // CMD_DESTROY_PATH
    "Show the path",            // CMD_SHOW_PATH
    "Start to follow the path", // CMD_START_PATH
    "You can save your path",               // CMD_SAVE_PATH
    "You can load your path",              // CMD_LOAD_PATH
    "Quit"                      // CMD_START_QUIT
};

static const char *success_messages[] = {
    "Path created",                     // CMD_CREATE_PATH
    "Step added successfully.",         // CMD_ADD_STEP
    "Path destroyed.",                  // CMD_PATH_DESTROYED
    "The current path is :",            // CMD_SHOW_PATH
    "Let's go\npress CTRL+C to exit", // CMD_START_PATH
    "Path saved successfully",          // CMD_SAVE_PATH
    "Path loaded successfully",         // CMD_LOAD_PATH
    "Bye"                               // CMD_QUIT
};

static const char *failure_messages[] = {
    "Failed to create path",                   // CMD_CREATE_PATH
    "Failed to add step, path isn't created.", // CMD_ADD_STEP
    "path already destroyed",                  // CMD_DESTROY_PATH
    "Path is not defined",                     // CMD_SHOW_PATH
    "Path is not defined",                     // CMD_START_PATH
    "Path is not saved successfully",          // CMD_SAVE_PATH
    "Path is not loaded successfully",         // CMD_LOAD_PATH
    "" // No failure message for CMD_QUIT
};


void print_prompt_message(id_message_prompt id) {
    printf("%s\n", prompt_messages[id]);
}

void print_success_message(menu_command cmd) {
    printf("%s\n", success_messages[cmd]);
}

void print_failure_message(menu_command cmd) {
    printf("%s\n", failure_messages[cmd]);
}

void print_menu() {
    print_prompt_message(UI_WELCOME);
    for (int i = 0; i < NB_COMMAND; i++) {
        printf("%d. %s\n", i, menu_strings[i]);
    }
} 