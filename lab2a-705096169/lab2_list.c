/*
    Name: Henry MacArthur
    Email: HenryMac16@gmail.com
    ID: 705096169
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>
#include "SortedList.h"

int num_iterations = 1;
int opt_yield = 0;
SortedList_t* list_head; //this is the first, aka, dummy node
SortedListElement_t* list_elements; //actual list items
pthread_mutex_t list_lock;
int is_sync = 0;
char sync_option;
int global_sync_lock = 0;
int total_elements;
char ** keys_array;

void * helper_func(void * ptr);

void free_memory()
{
    free(list_head);
    free(list_elements);
    int i = 0;
    for(; i < total_elements; i++)
    {
        free(keys_array[i]);
    }
    free(keys_array);
}
void catch_segmentation()
{
    fprintf(stderr, "Segmentation Error Caught! \n");
    //free_memory();
    exit(2);
}
void raise_err()
{
    fprintf(stderr, "err: %s \n", strerror(errno));
    free_memory();
    exit(1);
}
void print_err(char * msg)
{
    fprintf(stderr, "err: %s \n", msg);
    free_memory();
    exit(2);
}


int main(int argc, char ** argv)
{
    signal(SIGSEGV, catch_segmentation);

    static struct option long_options[] = {
        {"threads", required_argument, NULL, '1'},
        {"iterations", required_argument, NULL, '2'},
        {"yield", required_argument, NULL, '3'},
        {"sync", required_argument, NULL, '4'},
        {0,         0,                 NULL,  0 }
    };

    struct timespec start_time, end_time;
    char * short_options = "";
    int option_index = 0;
    int num_threads = 1;

    int c;
    while((c = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1)
    {
        switch(c)
        {
            case '1':
                num_threads = atoi(optarg);
                break;
            case '2':
                num_iterations = atoi(optarg);
                break;
            case '3': //do something based on what the input is
            {
                int len; 
                len = strlen(optarg);
                int i;
                for(i = 0; i < len; i++)
                {
                    switch(optarg[i])
                    {
                        case 'i':
                            opt_yield |= INSERT_YIELD;
                            break;
                        case 'd':
                            opt_yield |= DELETE_YIELD;
                            break;
                        case 'l':
                            opt_yield |= LOOKUP_YIELD;
                            break;
                        default:
                            fprintf(stderr, "unrecognized input! \n");
                            exit(1);
                    }
                }
                break;
            }
            case '4':
            {
                if((strcmp(optarg, "m") != 0) && (strcmp(optarg, "s") != 0))
                {
                    fprintf(stderr, "unrecognized inputs! \n");
                    exit(1);
                }
                else //valid input
                {
                    is_sync = 1;
                    sync_option = (char)optarg[0];
                }
                break;
            }
            default:
                fprintf(stderr, "unrecognized input! \n");
                exit(1);

        }
    }
    if(optind < argc)
    {
        fprintf(stderr, "unrecognized input! \n");
        exit(1);
    }
    //need to create threads x iterations elements 
    total_elements = num_threads * num_iterations;
    list_head = (SortedList_t *)malloc(sizeof(SortedList_t));
    //need to make key null, and initialize pointers
    list_head->next = NULL;
    list_head->prev = NULL;
    list_head->key = NULL;
    //now malloc the rest of the list
    list_elements = (SortedListElement_t *)malloc(sizeof(SortedListElement_t) * total_elements);

    //srand(1000);

    keys_array = malloc(sizeof(char *) * total_elements);

    int i = 0;
    for(; i < total_elements; i++) //loop through and generate random keys
    {
         //pick random number through 0 and 9
        int length = 3 + rand() % 10; //pick random key length, 5 - 15
        char * temp_key = (char *)malloc(length + 1); //save room for nullbyte
        keys_array[i] = temp_key;
        int j = 0;
        for(; j < length; j++)
        {
            int num = 0 + rand() % 9;
            temp_key[j] = num + '0';
        }
        temp_key[length] = '\0';
        //now i have my random key, set the list elements key to it
        list_elements[i].key = temp_key;
    }

    pthread_t * threads_array = (pthread_t *)malloc(num_threads * sizeof(pthread_t));

    //get start time
    if(clock_gettime(CLOCK_REALTIME, &start_time) < 0)
        raise_err();
    //time to create some threads and give them some elements
    i = 0;
    for(; i < num_threads; i++)
    { //send the first index of the items it should have, (list_elements) * (num_iterations + i)
        if(pthread_create(&threads_array[i], NULL, helper_func, (void *)(list_elements + (i *num_iterations))))
            //print_err("thread error");
            raise_err();
    }
    
    i = 0;
    for(; i < num_threads; i++)
    {
        if(pthread_join(threads_array[i], NULL))
        {
            raise_err();
            //print_err("thread error");
        }
    }

    if(clock_gettime(CLOCK_REALTIME, &end_time) < 0)
        raise_err();

    long long op = num_threads * num_iterations * 3;
    uint64_t total_times = 1000000000L * (end_time.tv_sec - start_time.tv_sec) + end_time.tv_nsec - start_time.tv_nsec;//end_time.tv_nsec - start_time.tv_nsec;
    long long unsigned int total_time= (long long unsigned int)total_times;
    unsigned int avg_op =  (unsigned int)(total_time/op);

    if(SortedList_length(list_head) != 0)
        print_err("list corruption"); //i should exit with an error here i think?
    
    char * test_name = "list-";
    char yield_opts[5] = "none";
    int k = 0;
    char * sync_opts = "-none"; //change this later
    if(is_sync && sync_option == 'm')
    {
        // sync_opts[1] = 'm';
        // sync_opts[2] = '\0';
        sync_opts = "-m";
    }
    else if(is_sync && sync_option == 's')
    {
        // sync_opts[1] = 's';
        // sync_opts[2] = '\0';
        sync_opts = "-s";
    }

    if(opt_yield & INSERT_YIELD) //i
    {
        yield_opts[k] = 'i';
        k++;
    }
    if(opt_yield & DELETE_YIELD) //d
    {
        yield_opts[k] = 'd';
        k++;
    }
    if(opt_yield & LOOKUP_YIELD) //l
    {
        yield_opts[k] = 'l';
        k++;
    }

    if(k != 0)
        yield_opts[k] = '\0';

    printf("%s%s%s,%d,%d,1,%lld,%lld,%d\n", test_name, yield_opts, sync_opts, num_threads, num_iterations, op, total_time, avg_op);

    free_memory();
    exit(0);
}
///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////
void insert_elements(SortedListElement_t * current)
{
    //printf("%d \n", num_iterations);
    int i = 0;
    for(; i < num_iterations; i++)
    {   
        SortedList_insert(list_head, &current[i]);
    }
}

void remove_elements(SortedListElement_t * current)
{
    int i = 0;
    for(; i < num_iterations; i++)
    {
        //printf("%s \n", current[i].key);
        SortedListElement_t * look_up = SortedList_lookup(list_head, current[i].key);

        if(look_up == NULL)
        {
            print_err("list corruption");
        }
        
        if(SortedList_delete(look_up) == 1)
        {
            print_err("list corruption");
        }
       
    }
}

void * helper_func(void * ptr)
{
    SortedListElement_t * start = (SortedListElement_t *)ptr;

    if(is_sync && sync_option == 'm') //for mutex
    {
        pthread_mutex_lock(&list_lock);
    }
    else if(is_sync && sync_option == 's')
    {
        while(__sync_lock_test_and_set(&global_sync_lock, 1));
    }
    //////
    insert_elements(start); //insert the respective items
    //////

    //////////
    int length = SortedList_length(list_head);
    if(length == -1)
    {
        print_err("list corruption");
    }
    ///////////

    ///////////
    remove_elements(start);
    ///////////
    if(is_sync && sync_option == 'm')
        pthread_mutex_unlock(&list_lock);
    else if(is_sync && sync_option == 's')
    {
        __sync_lock_release(&global_sync_lock);
    }
    
    return NULL;
}