#include "KmyShell.h"

int main()
{
    // all in a loop , this follow:
        // 0. >_
        // 1. getline() - Kshell_read_line();
        // 2. spilt() - Kshell_spilt_line();
        // 3. run() - Kshell_execute();

    Ksh_loop();

    return 0;
}