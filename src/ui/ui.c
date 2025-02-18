// UI.c

#include "ui.h"
#include "../robot_app/copilot.h"
#include "../robot_app/pilot.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../utils.h"
#define DELAY 100000
#define PATH_FILENAME "path.txt"

static int steps_number = 0;

// Menu options
typedef enum {
  CMD_CREATE_PATH,
  CMD_ADD_STEP,
  CMD_DESTROY_PATH,
  CMD_SHOW_PATH,
  CMD_START_PATH,
  CMD_SAVE_PATH,
  CMD_LOAD_PATH,
  CMD_QUIT,
  NB_COMMAND
} menu_command;

const char *menu_strings[] = {
    "Create a path",            // CMD_CREATE_PATH
    "Add a step to the Path",   // CMD_ADD_STEP
    "Destroy the path",         // CMD_DESTROY_PATH
    "Show the path",            // CMD_SHOW_PATH
    "Start to follow the path", // CMD_START_PATH
    "You can save your path",               // CMD_SAVE_PATH
    "You can load your path",              // CMD_LOAD_PATH
    "Quit"                      // CMD_START_QUIT
};

const char *success_messages[] = {
    "Path created",                     // CMD_CREATE_PATH
    "Step added successfully.",         // CMD_ADD_STEP
    "Path destroyed.",                  // CMD_PATH_DESTROYED
    "The current path is :",            // CMD_SHOW_PATH
    "Let's go\npress CTRL+C to exit", // CMD_START_PATH
    "Path saved successfully",          // CMD_SAVE_PATH
    "Path loaded successfully",         // CMD_LOAD_PATH
    "Bye"                               // CMD_QUIT
};

const char *failure_messages[] = {
    "Failed to create path",                   // CMD_CREATE_PATH
    "Failed to add step, path isn't created.", // CMD_ADD_STEP
    "path already destroyed",                  // CMD_DESTROY_PATH
    "Path is not defined",                     // CMD_SHOW_PATH
    "Path is not defined",                     // CMD_START_PATH
    "Path is not saved successfully",          // CMD_SAVE_PATH
    "Path is not loaded successfully",         // CMD_LOAD_PATH
    "" // No failure message for CMD_QUIT
};

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

const char *prompt_messages[] = {
    "\n=== User Interface ===\n",
    "Enter your choice",
    "Enter the numer of step in the path",
    "Enter the step number",
    "Enter the speed",
    "Enter the type of move (0:Forward, 1:Rotation)",
    "Enter the distance to go forward",
    "Enter the type of rotation (1:left, 2:right, 3: flip turn)"};

static void print_prompt_message(id_message_prompt id) {
  printf("%s\n", prompt_messages[id]);
}

static void print_success_message(menu_command cmd) {
  printf("%s\n", success_messages[cmd]);
}

static void print_failure_message(menu_command cmd) {
  printf("%s\n", failure_messages[cmd]);
}

static void print_menu() {
  print_prompt_message(UI_WELCOME);
  for (int i = 0; i < NB_COMMAND; i++) {
    printf("%d. %s\n", i, menu_strings[i]);
  }
}

/*Function to create the amount of steps*/
static int handle_user_prompt_int(id_message_prompt id, int min, int max) {
  int value = 0;
  do {
    print_prompt_message(id);
    printf("Enter a value between %d and %d\n", min, max);
    if(scanf("%d",&value) != 1){
      printf("Error : please enter a int value\n"); 
        while(getchar()!='\n'); 
    }
    getchar();           // pour se débarasser du retour ligne
  } while ((value < min) || (value > max));
  return value;
}

static void handle_create_path() {
  /*TODO: prompt for the number of steps and create an empty path, print success
   * or failure messages accordingly */
  static int value;
  value = handle_user_prompt_int(UI_ASK_SIZE_PATH,0,100);
  if(value >0 && value <=100){
    steps_number = value;
    print_success_message(CMD_CREATE_PATH);
  }else{
    steps_number = 0;
    print_failure_message(CMD_CREATE_PATH);
  }
  move_t* step;
  step = calloc(steps_number,sizeof(move_t));
  copilot_set_Path(step,steps_number);
  /*HINT: update steps_number on success or set to 0 on failure */
}

static void handle_add_step() {
  /*TODO: prompt for adequate data to call copilot_add_step(index,step); */
  /*HINT: adding step fails if steps_number == 0 */
  if(steps_number == 0){
    printf("Error : Impossible to access to the path\n");
    return;
  }
  move_t* step = copilot_get_path();
  
    for(int i=0;i<steps_number;i++){
      printf("\n=== Configuration of the step %d\n",i+1);

      int move_choice;
      do{
        move_choice = handle_user_prompt_int(UI_ASK_TYPE_MOVE,0,1);
        if(move_choice!=0 && move_choice !=1){
          printf("Error : please chose the value 0 to go forward or 1 to turn\n"); 
        }

      }while (move_choice!=0 && move_choice !=1); 
      step[i].move_type = (move_choice == 0) ? FORWARD : ROTATION;

        /* Configuration of speed*/
        
      int speed;
      do{
        print_prompt_message(UI_ASK_SPEED);
        printf("Between -100 and 100\n");
        if(scanf("%d",&speed)!=1){
          printf("Error : please enter a int value\n"); 
          while(getchar()!='\n'); 
        }
        getchar(); 
        if(speed<-100 || speed>100){
          printf("Error : please chose a value between -100 and 100\n");
        }
      }while(speed <-100 || speed>100);
      step[i].speed = speed;

      if(step[i].move_type == FORWARD){
        int distance; 
        do{
          print_prompt_message(UI_ASK_VALUE_FORWARD);
          printf(("Only positive value\n"));
          if(scanf("%d",&distance)==0){
            printf("Error : please enter a int value\n"); 
            while(getchar()!='\n');
          }
          getchar();
          if(distance<=0){
            printf("Error : please chose a positive value\n"); 
          }
        }while(distance<=0);
        step[i].range.distance = distance; 
        printf("Configuration : distance %d, speed %d\n",step[i].range.distance,step[i].speed);
      }else{
        int rotation_choice; 
        do{
          print_prompt_message(UI_ASK_VALUE_TURN); 
          printf("1 : Left, 2 : Right, 3 : U_TURN\n"); 
          if(scanf("%d",&rotation_choice)!=1){
            printf("Error : please enter a int value\n"); 
            while(getchar()!='\n');
          } 
          getchar(); 
          if(rotation_choice != 1 && rotation_choice != 2 && rotation_choice != 3){
            printf("Error : please chose one of the following value : \n"); 
            printf("1 : Right\n 2 : Left\n 3 : U_Turn\n"); 
          }
        }while(rotation_choice <1 && rotation_choice > 3); 

        switch(rotation_choice){
          case 1 : 
            step[i].range.angle = RIGHT; 
            printf("Configuration : Right, speed %d\n",step[i].speed); 
            break; 
          case 2 : 
            step[i].range.angle = LEFT; 
            printf("Configuration : Left, speed %d\n",step[i].speed); 
            break; 
          case 3 : 
            step[i].range.angle = U_TURN; 
            printf("Configuration : U_TURN, speed %d\n",step[i].speed); 
            break;
          default : 
            printf("Error : the value is not correct\n"); 
            break; 
        }
      }
  }
  
  print_success_message(CMD_ADD_STEP);
}

static void handle_destroy_path() {
  /*TODO: call copilot_destroy_path(); */
  int etat = copilot_destroy_path();
  if(etat !=0){
    print_failure_message(CMD_DESTROY_PATH);
  }else{
    print_success_message(CMD_DESTROY_PATH);
  }
  /*HINT: steps_number is set to 0 on success to prevent adding steps to a
   * non-existing path */
  steps_number = 0;
}

static int handle_start_path() {
  /*TODO: exit ui to start path */
  move_t* current_path = copilot_get_path();
  if(steps_number > 0 && current_path!=NULL){
    /*Starting the robot*/
    copilot_start_path(); 
    print_success_message(CMD_START_PATH);

    while(!copilot_is_path_complete()){
      copilot_stop_at_step_completion();
      usleep(DELAY);
    }
    return EXIT_SUCCESS;
  }
  print_failure_message(CMD_START_PATH);
  return EXIT_FAILURE;
  /*HINT: how could you detect an empty path? (created but with no added steps?)
   */
}

static void handle_save_path(){
    /*Verifying that a path  exists*/
    if(steps_number == 0 || copilot_get_path() == NULL){
      print_failure_message(CMD_SAVE_PATH); 
      return; 
    }

    FILE* file = fopen(PATH_FILENAME,"w"); 
    if(file == NULL){
      printf("Error : Impossible to create the saving file\n");
      print_failure_message(CMD_SAVE_PATH);
      return; 
    }

    /*Saving of number of steps*/
    fprintf(file,"%d\n",steps_number); 

    /*Saving of each steps*/
    move_t* save_path = copilot_get_path();
    for(int i = 0; i<steps_number; i++){
      fprintf(file,"%d\n",save_path[i].speed);
      if(save_path[i].move_type == FORWARD){
        fprintf(file,"FORWARD %d",save_path[i].range.distance); 
      }else{
        fprintf(file,"ROTATION %d",save_path[i].range.angle);
      }
    }
    fclose(file); 
    print_success_message(CMD_SAVE_PATH);
    
}

static void handle_load_path(){
    
    /*Reading of the path*/
    FILE* file = fopen(PATH_FILENAME,"r"); 
    if(file == NULL){
      printf("Error : can't find file"); 
      print_failure_message(CMD_LOAD_PATH);
      return; 
    }

    /*Reading of the steps*/
    int load_steps; 
    if(fscanf(file,"%d\n",&load_steps) != 1){
      printf("Error : Format de fichier incorrect"); 
      fclose(file); 
      print_failure_message(CMD_LOAD_PATH);
      return; 
    }

    move_t* load_path = calloc(load_steps,sizeof(move_t)); 
    if(load_path == NULL){
      printf("Error : Impossible to allocate memory\n"); 
      fclose(file); 
      print_failure_message(CMD_LOAD_PATH);
      return; 
    }

    /*Lecture des steps*/
    int move_type, speed, value; 
    for(int i =0; i < load_steps; i++){
      if(fscanf(file,"%d %d %d",&move_type,&value,&speed) != 3){
        printf("Error : Reading step %d impossible", i+1);
        free(load_path);
        fclose(file); 
        print_failure_message(CMD_LOAD_PATH);
        return;
      }
      load_path[i].speed = speed; 
      load_path[i].move_type = move_type;
      if(move_type == FORWARD){
        load_path[i].range.distance = value; 
      }else{
        load_path[i].range.angle = value;  
      }
    }
    fclose(file); 

    if(copilot_get_path()!=NULL){
      copilot_destroy_path();
    }
    steps_number = load_steps; 
    copilot_set_Path(load_path,steps_number);
    print_success_message(CMD_LOAD_PATH);

}

static void handle_show_path() {
  int i = 0;
  move_t step;
  printf("You chose to show the path\n");
  if (steps_number > 0) {
    print_success_message(CMD_SHOW_PATH);
    for (i = 0; i < steps_number; i++) {
      step = copilot_get_step(i);
      if (step.move_type == FORWARD)
        printf("%d : FORWARD distance:%d \n", i, step.range.distance);
      else
        printf("%d : ROTATION turn: %d \n", i, step.range.angle);
    }
  } else {
    print_failure_message(CMD_SHOW_PATH);
  }
}

extern int ui_start() {
  int choice;
  // User interface loop
  while (1) {
    print_menu();
    choice = handle_user_prompt_int(UI_ASK_CHOICE, 0, (NB_COMMAND - 1));
    printf("Value of the user's choice : %d\n",choice);
    switch (choice) {
      case CMD_CREATE_PATH:
        handle_create_path();
        break;
      case CMD_ADD_STEP:
        handle_add_step();
        break;
      case CMD_DESTROY_PATH:
        handle_destroy_path();
        break;
      case CMD_SHOW_PATH:
        printf("I go into the ui_start for handle_show_path");
        handle_show_path();
        break;
      case CMD_START_PATH:
        if (handle_start_path() == EXIT_SUCCESS)
          return EXIT_SUCCESS;
        break;
      case CMD_SAVE_PATH:
        handle_save_path();
        break;
      case CMD_LOAD_PATH:
        handle_load_path();
        break;
      case CMD_QUIT:
        print_success_message(CMD_QUIT);
        return EXIT_FAILURE;
      default:
        break;
    }
  }
  return EXIT_SUCCESS; // never used
}
