#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>

#include "display.h"

void dbg(const char* msg, ...){
    printf(ANSI_COLOR_GREEN "\n[Debug] ");
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    printf(ANSI_COLOR_RESET);
}

void info(const char* msg, ...){
    printf(ANSI_COLOR_BLUE "\n[Info] ");
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    printf(ANSI_COLOR_RESET);
}

void err(const char* msg, ...){
    printf(ANSI_COLOR_RED "\n[Error] ");
    va_list args;
    va_start(args, msg);
    vprintf(msg,args);    
    printf(ANSI_COLOR_RESET);
}

void warn(const char* msg, ...){
    printf(ANSI_COLOR_YELLOW "\n[Warning] ");
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    printf("\n" ANSI_COLOR_RESET);
}
