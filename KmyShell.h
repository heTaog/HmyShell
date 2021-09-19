#ifndef KMYSHELL_H
#define KMYSHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/wait.h>

enum OPTION {
    ERROR = -1,
    QUIT  =  0,
    OK    =  1
};

void sys_err(const char* ptr);

char*  Ksh_read_line();
char** Ksh_spilt_line(char* line);
int    Ksh_execv(char** args);
int    Ksh_launch(char** args);

void Ksh_loop();

// some function of control for KmyShell
int Ksh_quit();
int Ksh_help();

#endif  // KmyShell.h