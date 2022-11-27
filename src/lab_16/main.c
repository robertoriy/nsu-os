#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <semaphore.h>
#include "error_check.h"


int main(int argc, char** argv)
{
    if (2 != argc)
    {
        fprintf(stderr, "Expected one argument\n");
        return EXIT_FAILURE;
    }
    int NUMBER_OF_LINES = atoi(argv[1]);
    if (NUMBER_OF_LINES < 1 || NUMBER_OF_LINES > 50000)
    {
        fprintf(stderr, "Expected correct argument: [1:50000]\n");
        return EXIT_FAILURE;
    }

    char* PARENT_TEXT = "[parent]";
	char* CHILD_TEXT = "[child]";


    sem_t *sem_parent = sem_open("/sem_parent", O_CREAT, 0600, 0);
    sem_t *sem_child = sem_open("/sem_child", O_CREAT, 0600, 1);
    if (sem_parent == SEM_FAILED || sem_child == SEM_FAILED)
    {
        fprintf(stderr, "sem_open() failed\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (-1 == pid)
    {
        fprintf(stderr, "fork() failed\n");
        sem_close(sem_parent);
        sem_close(sem_child);
        return EXIT_FAILURE;
    } else if (0 == pid)
    {
        for (int i = 0; i < NUMBER_OF_LINES; ++i)
        {
            CHCK_SYNC_OBJ(sem_wait(sem_child));
            printf("Written by %s - %d\n", CHILD_TEXT, i + 1);
            CHCK_SYNC_OBJ(sem_post(sem_parent));
        }
    } else
    {
        for (int i = 0; i < NUMBER_OF_LINES; ++i)
        {
            CHCK_SYNC_OBJ(sem_wait(sem_parent));
            printf("Written by %s - %d\n\n", PARENT_TEXT, i + 1);
            CHCK_SYNC_OBJ(sem_post(sem_child));
        }
    }

    sem_close(sem_parent);
    sem_close(sem_child);

	return EXIT_SUCCESS;
}
