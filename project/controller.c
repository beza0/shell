#include "controller.h"
#include <string.h>
#include <stdio.h>

char *handle_input(ShmBuf *shmp, const char *input) {
    if (strncmp(input, "@msg", 4) == 0) {
        model_send_message(shmp, input + 5);
        return strdup("[message sent]");
    } else {
        return execute_command(input);
    }
}

