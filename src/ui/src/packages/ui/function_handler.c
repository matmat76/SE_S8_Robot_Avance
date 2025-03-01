#include "function_handler.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "text_prompts.h"
#include "../../../../common/data-type.h"

#define DELAY 100000
#define PATH_FILENAME "path.txt"

static int steps_number = 0;

/*Function to create the amount of steps*/
int handle_user_prompt_int(id_message_prompt id, int min, int max)
{
    int value = 0;
    do
    {
        print_prompt_message(id);
        printf("Enter a value between %d and %d\n", min, max);
        if (scanf("%d", &value) != 1)
        {
            printf("Error : please enter a int value\n");
            while (getchar() != '\n')
                ;
        }
        getchar(); // pour se débarasser du retour ligne
    } while ((value < min) || (value > max));
    return value;
}

void handle_create_path()
{
    /*TODO: prompt for the number of steps and create an empty path, print success
     * or failure messages accordingly */
    static int value;
    value = handle_user_prompt_int(UI_ASK_SIZE_PATH, 0, 100);
    if (value > 0 && value <= 100)
    {
        steps_number = value;
        print_success_message(CMD_CREATE_PATH);
    }
    else
    {
        steps_number = 0;
        print_failure_message(CMD_CREATE_PATH);
    }
    move_t *step;
    step = calloc(steps_number, sizeof(move_t));
    copilot_set_Path(step, steps_number);
    /*HINT: update steps_number on success or set to 0 on failure */
}

void handle_add_step()
{
    /*TODO: prompt for adequate data to call copilot_add_step(index,step); */
    /*HINT: adding step fails if steps_number == 0 */
    if (steps_number == 0)
    {
        printf("Error : Impossible to access to the path\n");
        return;
    }
    move_t *step = copilot_get_path();

    for (int i = 0; i < steps_number; i++)
    {
        printf("\n=== Configuration of the step %d\n", i + 1);

        int move_choice;
        do
        {
            move_choice = handle_user_prompt_int(UI_ASK_TYPE_MOVE, 0, 1);
            if (move_choice != 0 && move_choice != 1)
            {
                printf("Error : please chose the value 0 to go forward or 1 to turn\n");
            }

        } while (move_choice != 0 && move_choice != 1);
        step[i].move_type = (move_choice == 0) ? FORWARD : ROTATION;

        /* Configuration of speed*/

        int speed;
        do
        {
            print_prompt_message(UI_ASK_SPEED);
            printf("Between -100 and 100\n");
            if (scanf("%d", &speed) != 1)
            {
                printf("Error : please enter a int value\n");
                while (getchar() != '\n')
                    ;
            }
            getchar();
            if (speed < -100 || speed > 100)
            {
                printf("Error : please chose a value between -100 and 100\n");
            }
        } while (speed < -100 || speed > 100);
        step[i].speed = speed;

        if (step[i].move_type == FORWARD)
        {
            int distance;
            do
            {
                print_prompt_message(UI_ASK_VALUE_FORWARD);
                printf(("Only positive value\n"));
                if (scanf("%d", &distance) == 0)
                {
                    printf("Error : please enter a int value\n");
                    while (getchar() != '\n')
                        ;
                }
                getchar();
                if (distance <= 0)
                {
                    printf("Error : please chose a positive value\n");
                }
            } while (distance <= 0);
            step[i].range.distance = distance;
            printf("Configuration : distance %d, speed %d\n", step[i].range.distance, step[i].speed);
        }
        else
        {
            int rotation_choice;
            do
            {
                print_prompt_message(UI_ASK_VALUE_TURN);
                printf("1 : Left, 2 : Right, 3 : U_TURN\n");
                if (scanf("%d", &rotation_choice) != 1)
                {
                    printf("Error : please enter a int value\n");
                    while (getchar() != '\n')
                        ;
                }
                getchar();
                if (rotation_choice != 1 && rotation_choice != 2 && rotation_choice != 3)
                {
                    printf("Error : please chose one of the following value : \n");
                    printf("1 : Right\n 2 : Left\n 3 : U_Turn\n");
                }
            } while (rotation_choice < 1 && rotation_choice > 3);

            switch (rotation_choice)
            {
            case 1:
                step[i].range.angle = RIGHT;
                printf("Configuration : Right, speed %d\n", step[i].speed);
                break;
            case 2:
                step[i].range.angle = LEFT;
                printf("Configuration : Left, speed %d\n", step[i].speed);
                break;
            case 3:
                step[i].range.angle = U_TURN;
                printf("Configuration : U_TURN, speed %d\n", step[i].speed);
                break;
            default:
                printf("Error : the value is not correct\n");
                break;
            }
        }
    }

    print_success_message(CMD_ADD_STEP);
}

void handle_destroy_path()
{
    /*TODO: call copilot_destroy_path(); */
    int etat = copilot_destroy_path();
    if (etat != 0)
    {
        print_failure_message(CMD_DESTROY_PATH);
    }
    else
    {
        print_success_message(CMD_DESTROY_PATH);
    }
    /*HINT: steps_number is set to 0 on success to prevent adding steps to a
     * non-existing path */
    steps_number = 0;
}

int handle_start_path()
{
    /*TODO: exit ui to start path */
    move_t *current_path = copilot_get_path();
    if (steps_number > 0 && current_path != NULL)
    {
        /*Starting the robot*/
        copilot_start_path();
        print_success_message(CMD_START_PATH);

        while (!copilot_is_path_complete())
        {
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