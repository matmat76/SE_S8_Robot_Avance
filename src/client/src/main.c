#include <stdio.h>
#include <stdlib.h>

#include "./packages/socket/sock.h"
#include "./packages/ui/ui.h"

int main() {
    sock_init();
    sock_connect();
    // Start the user interface
    if (ui_start() == EXIT_SUCCESS) {
        printf("Program exited successfully.\n");
    } else {
        printf("Program terminated with errors.\n");
    }
    sock_close();
    return 0;
}