#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

char check_filetype(char *fullname);

int search(char *folder);

void add_directories(int argc, char **argv, int optind);

void *main_thread(void *thr_id);

void init_vars(int argc, char **argv);

int main(int argc, char **argv);
