#include <stdio.h>
#include <stdlib.h>
#include "./packages/ui/ui.h"

int main() {
    // Start the user interface
    if (ui_start() == EXIT_SUCCESS) {
        printf("Program exited successfully.\n");
    } else {
        printf("Program terminated with errors.\n");
    }
    return 0;
}