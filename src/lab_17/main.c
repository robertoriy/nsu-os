#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "error_check.h"

#define MAX_STRING_LENGTH 80
#define SORTING_PERIOD 5

typedef struct node 
{
    char* data;
    struct node* next;
} node;

node* head;
pthread_mutex_t list_mutex;

void* sorter_routine(void* argv);

node* add_node(node* head, char* data);
void sort_list(node* head);
void print_list(node* head);
void free_list(node* head);

int main(int argc, char** argv)
{
    if (1 != argc)
    {
        fprintf(stderr, "$~ Expected no arguments\n");
        return EXIT_FAILURE;
    }
    CHCK_FUNC(pthread_mutex_init(&list_mutex, NULL), return EXIT_FAILURE);

    pthread_t sorter;
    CHCK_FUNC(pthread_create(&sorter, NULL, sorter_routine, NULL), return EXIT_FAILURE);
    
    char buffer[MAX_STRING_LENGTH + 1] = {0};
    while (true)
    {
        if (NULL == fgets(buffer, MAX_STRING_LENGTH, stdin))
        {
            fprintf(stderr, "$~ EOF occured\n");
            break;
        } 
        CHCK_SYNC_OBJ(pthread_mutex_lock(&list_mutex));
        if (0 == strcmp(buffer, "\n")) 
        {      
            print_list(head);
        } else
        {            
            head = add_node(head, buffer);
        }
        CHCK_SYNC_OBJ(pthread_mutex_unlock(&list_mutex));
    }
    CHCK_FUNC(pthread_cancel(sorter), return EXIT_FAILURE);

    CHCK_SYNC_OBJ(pthread_mutex_lock(&list_mutex));    
    print_list(head);
    CHCK_SYNC_OBJ(pthread_mutex_unlock(&list_mutex));

    free_list(head);
    CHCK_FUNC(pthread_mutex_destroy(&list_mutex), return EXIT_FAILURE);
	return EXIT_SUCCESS;
}


void* sorter_routine(void* argv)
{
    while (true)
    {
        sleep(SORTING_PERIOD);

        CHCK_SYNC_OBJ(pthread_mutex_lock(&list_mutex));
        printf("\nSorting the list....\n\n");
        sort_list(head);
        printf("\nThe list has been sorted.\n\n");
        CHCK_SYNC_OBJ(pthread_mutex_unlock(&list_mutex));
    }

	return NULL;
}


node* add_node(node* current_head, char* data)
{
    size_t length = strlen(data);
    if (length > MAX_STRING_LENGTH)
    {
        fprintf(stderr, "Expected string with length less than - %d\n", MAX_STRING_LENGTH + 1);
        return current_head;
    }
    node* new_head = (node*)malloc(sizeof(node));
    new_head->data = (char*)calloc(MAX_STRING_LENGTH + 1, sizeof(char));
    new_head->next = current_head;
    strncpy(new_head->data, data, length);
    return new_head;
}

void sort_list(node* head)
{
    if (NULL == head)
    {
        return;
    }
    node *left = head;
    node *right = head->next;

    node *temp = (node *)malloc(sizeof(node));
    temp->data = (char *)calloc(MAX_STRING_LENGTH + 1, sizeof(char));
    
    while (NULL != left->next)
    {
        while (NULL != right)
        {
            if (strncmp(left->data, right->data, MAX_STRING_LENGTH) > 0)
            {
                strncpy(temp->data, left->data, MAX_STRING_LENGTH);
                strncpy(left->data, right->data, MAX_STRING_LENGTH);
                strncpy(right->data, temp->data, MAX_STRING_LENGTH);
            }
            right = right->next;
        }
        left = left->next;
        right = left->next;
    }
    free(temp->data);
    free(temp);
}


void print_list(node* head)
{
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n >THE LIST CONTAINS:\n");
    node* current = head;
    while (NULL != current)
    {
        printf("\t%s", current->data);
        current = current->next;
    }
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void free_list(node* head)
{
    while (NULL != head)
    {
        node* temp = head;
        head = head->next;
        if (NULL != temp->data)
        {
            free(temp->data);
        }
        free(temp);
    }
}