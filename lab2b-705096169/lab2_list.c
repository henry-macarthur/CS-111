#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include "SortedList.h"
/*
#NAME: HENRY MACARTHUR
#EMAIL: HENRYMAC16@GMAIL.COM
#ID: 705096169

*/
int num_iterations = 1;
int opt_yield = 0;
//SortedList_t* list_head; //this is the first, aka, dummy node
SortedListElement_t* list_elements; //actual list items
pthread_mutex_t list_lock;
int is_sync = 0;
char sync_option;
int global_sync_lock = 0;
int total_elements;
char ** keys_array;
int num_lists = 1;
pthread_mutex_t * list_locks;

int * sync_locks;

SortedList_t** head_arr;
int get_lengths();
//have an array of linked list heads, so array of pointers to list heads
//[]->.->.
//[]->.
//[]->.->.->.
//[]->.->.
//

void * helperfunc(void * ptr);

void free_memory()
{
    //free(list_head);
    free(list_elements);
    int i = 0;
    for(; i < total_elements; i++)
    {
        free(keys_array[i]);
    }
    free(keys_array);
    free(sync_locks);
    free(list_locks);

    i = 0;
    for(; i < num_lists; i++)
    {
        free(head_arr[i]);
    }
    free(head_arr);
}
void catch_segmentation()
{
    fprintf(stderr, "Segmentation Error Caught! \n");
    free_memory();
    exit(1);
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
        {"lists", optional_argument, NULL, '5'},
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
            case '5':
                num_lists = atoi(optarg);
                break;
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

    head_arr = malloc(sizeof(SortedList_t*) * num_lists);
    sync_locks = calloc(num_lists, sizeof(int)); //set all to 0
    //need to create threads x iterations elements 
    total_elements = num_threads * num_iterations;

    //need to make key null, and initialize pointers
    int j = 0;
    for(; j < num_lists; j++)
    {
        head_arr[j] = malloc(sizeof(SortedList_t));
        head_arr[j]->next = NULL;
        head_arr[j]->prev = NULL;
        head_arr[j]->key = "asdasdad";//NULL;
    }


    //now malloc the rest of the list
    list_elements = (SortedListElement_t *)malloc(sizeof(SortedListElement_t) * total_elements);
    list_locks = malloc(sizeof(pthread_mutex_t) * num_lists); //make a list of locks, one for each sublist
    
    j = 0;
    for(; j < total_elements; j++)
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
        if(pthread_create(&threads_array[i], NULL, helperfunc, (void *)(list_elements + (i *num_iterations))))
            //print_err("thread error");
            raise_err();
    }
    
    int item = 5;
    void * ptr1 = (void *)&item;

    long long total_thread_time = 0;
    void ** individual_thread_time = (void **)&ptr1;
    i = 0;
    for(; i < num_threads; i++)
    {
        if(pthread_join(threads_array[i], individual_thread_time))
        {
            raise_err();
            //print_err("thread error");
        }
        total_thread_time += (long long) *individual_thread_time;
    }


    if(clock_gettime(CLOCK_REALTIME, &end_time) < 0)
        raise_err();

    long long op = num_threads * num_iterations * 3;
    long long total_time = 1000000000L * (end_time.tv_sec - start_time.tv_sec) + end_time.tv_nsec - start_time.tv_nsec;//end_time.tv_nsec - start_time.tv_nsec;
    int avg_op =  total_time/op;

    //fprintf(stderr, "join thresssads \n");

    if(get_lengths(NULL) != 0)
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

    printf("%s%s%s,%d,%d,%d,%lld,%lld,%d,%lld\n", test_name, yield_opts, sync_opts, num_threads, num_iterations, num_lists, op, total_time, avg_op, total_thread_time/op);

    free_memory();
    exit(0);
}
///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////

int get_hash(const char * itr)
{
    int hash = 0;
    int len = strlen(itr);
    int k = 0;
    for(; k < len; k++)
    {
        hash += (itr[k] - '0'); //convert to its integer value and then sum
    }
    return hash % num_lists; //insert into this sublist
}
long long insert_elements(SortedListElement_t * current)
{
    struct timespec thread_time_start, thread_time_end;
    //simplest hash should just be a sum of all the elements?
    long long time = 0;
    int i = 0;
    for(; i < num_iterations; i++)
    {
        
        int list_id = get_hash(current[i].key);
        //pthread_mutex_lock(&list_locks[0]);
        //want to get lock 
       
        if(is_sync && clock_gettime(CLOCK_REALTIME, &thread_time_start) < 0)
            raise_err();
        if(is_sync && sync_option == 'm')
            pthread_mutex_lock(&list_locks[list_id]);
        else if(is_sync && sync_option == 's')
            while(__sync_lock_test_and_set(sync_locks + list_id, 1)); //sync_locks
        if(is_sync && clock_gettime(CLOCK_REALTIME, &thread_time_end) < 0)
            raise_err();
        time += 1000000000L * (thread_time_end.tv_sec - thread_time_start.tv_sec) + thread_time_end.tv_nsec - thread_time_start.tv_nsec;
        SortedList_insert(head_arr[list_id], &current[i]);
        //SortedList_insert(list_head, &current[i]);
        
       // pthread_mutex_unlock(&list_locks[0]);
        if(is_sync && sync_option == 'm')
            pthread_mutex_unlock(&list_locks[list_id]);
        else if(is_sync && sync_option == 's')
            __sync_lock_release(sync_locks + list_id);
        
    }
    return time;
}


long long remove_elements(SortedListElement_t * current)
{
    struct timespec thread_time_start, thread_time_end;
    long long time = 0;
    int i = 0;
    for(; i < num_iterations; i++)
    {
        //printf("%s \n", current[i].key);
        int list_id = get_hash(current[i].key);

        if(is_sync && clock_gettime(CLOCK_REALTIME, &thread_time_start) < 0)
            raise_err();
        if(is_sync && sync_option == 'm')
            pthread_mutex_lock(&list_locks[list_id]);
        else if(is_sync && sync_option == 's')
            while(__sync_lock_test_and_set(sync_locks + list_id, 1));
        //SortedListElement_t * look_up = SortedList_lookup(list_head, current[i].key); 
        if(is_sync && clock_gettime(CLOCK_REALTIME, &thread_time_end) < 0)
            raise_err();

        time += 1000000000L * (thread_time_end.tv_sec - thread_time_start.tv_sec) + thread_time_end.tv_nsec - thread_time_start.tv_nsec;

        SortedListElement_t * look_up = SortedList_lookup(head_arr[list_id], current[i].key); 
        if(look_up == NULL)
        {
            print_err("list corruption");
        }

        
        if(SortedList_delete(look_up) == 1)
        {
            print_err("list corruption");
        }
        if(is_sync && sync_option == 'm')
            pthread_mutex_unlock(&list_locks[list_id]);
        else if(is_sync && sync_option == 's')
            __sync_lock_release(sync_locks + list_id);
    }
    
    return time;
}

//we should have a lock for length as it means no other thread can modify it while we getting length
int get_lengths(long long * counter)
{
    
    struct timespec thread_time_start, thread_time_end;
    long long time = 0;
    int length = 0;
    int k = 0;
    for(; k < num_lists; k++)
    {
        if(is_sync && clock_gettime(CLOCK_REALTIME, &thread_time_start) < 0)
            raise_err();
        if(is_sync && sync_option == 'm')
            pthread_mutex_lock(&list_locks[k]);
        else if(is_sync && sync_option == 's')
            while(__sync_lock_test_and_set(sync_locks + k, 1));
    
        if(is_sync && clock_gettime(CLOCK_REALTIME, &thread_time_end) < 0)
            raise_err();
        time += 1000000000L * (thread_time_end.tv_sec - thread_time_start.tv_sec) + thread_time_end.tv_nsec - thread_time_start.tv_nsec;
        
        int cur = SortedList_length(head_arr[k]);;

        if(cur == -1)
            print_err("list corruption");
        length += cur;

        if(is_sync && sync_option == 'm')
            pthread_mutex_unlock(&list_locks[k]);
        else if(is_sync && sync_option == 's')
            __sync_lock_release(sync_locks + k);
    }

    if(counter != NULL)
        *counter += time;
    //have to find a way to get total length
    return length;
}

//so the whole idea is that we want to lock each list head
void * helperfunc(void * ptr)
{
    SortedListElement_t * start = (SortedListElement_t *)ptr;
    // struct timespec thread_time_start, thread_time_end;
    //use these, pass them into each of the functions
    long long total_time = 0;
    // if(clock_gettime(CLOCK_REALTIME, &thread_time_start) < 0)
    //     raise_err();

    //have the lock, start the timer
    total_time += insert_elements(start); //insert the respective items
    //fprintf(stderr, "mmmm \n");
    
    // else if(is_sync && sync_option == 's')
    // {
    //     while(__sync_lock_test_and_set(&global_sync_lock, 1));
    // }
    int length = get_lengths(&total_time);
    // //replace this with the get_length function
    // int length = SortedList_length(list_head); //this is going to be different as well
    if(length == -1)
    {
        print_err("list corruption");
    }

    total_time += remove_elements(start);
    // else if(is_sync && sync_option == 's')
    // {
    //     __sync_lock_release(&global_sync_lock);
    // }

    // if(clock_gettime(CLOCK_REALTIME, &thread_time_end) < 0)
    //     raise_err();
    
    
    // if(is_sync)
    //     total_time = (1000000000L * (thread_time_end.tv_sec - thread_time_start.tv_sec) + thread_time_end.tv_nsec - thread_time_start.tv_nsec) / 3;

    return (void *)total_time;
}