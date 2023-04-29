
#include <stdio.h>

#define ERRO_FD  stderr
#define INFO_FD  stdout
#define DBUG_FD  stdout
#define WARN_FD  stdout

#define ERRO_COLOR   "\033[31m"
#define INFO_COLOR   "\033[32m"
#define DBUG_COLOR   "\033[34m"
#define WARN_COLOR   "\033[35m"
#define END_COLOR    "\033[0m"

#define __echo(type, format, ...)             \
    do {                                      \
        fprintf(type, format, ##__VA_ARGS__); \
    } while (0)

#define ECHO(type, format, ...)                                             \
    do {                                                                    \
        __echo(type ## _FD, "%s %4d " type ## _COLOR format "\n" END_COLOR, \
                "[" #type "] ", __LINE__, ##__VA_ARGS__);                   \
    } while (0)
