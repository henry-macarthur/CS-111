/*
    NAME: Henry MacArthur
    EMAIL: HenryMac16@gmail.com
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
#include <sched.h>

long long counter = 0; //this could be better practice using a struct and passing it around, but this should be fine
int opt_yield = 0;
pthread_mutex_t mutexsum;
int is_sync = 0;
char sync_option; //d for default

int global_sync_lock = 0;
int global_swap_lock = 0;

void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    if (opt_yield)
            sched_yield();
    *pointer = sum;
}
void add_mutex(long long *pointer, long long value) { //use mutexes!
    pthread_mutex_lock(&mutexsum); //lock, now enter critical section
    long long sum = *pointer + value;
    if (opt_yield)
            sched_yield();
    *pointer = sum;
    //can now leave critical section
    pthread_mutex_unlock(&mutexsum);
}
void add_sync(long long *pointer, long long value) {
    while(__sync_lock_test_and_set(&global_sync_lock, 1)); //an int should work fine? want to set to true
    //spin!
    long long sum = *pointer + value;
    if (opt_yield)
            sched_yield();
    *pointer = sum;
    __sync_lock_release(&global_sync_lock);
}
void add_compare(long long *pointer, long long value) {

    long long outdated_value;
    long long updated_value;
    do
    {
        outdated_value = *pointer;
        updated_value = outdated_value + value;
        if(opt_yield)
            sched_yield();
        //*pointer = updated_value; this must be commented as compare and swap does this for me
    } while(__sync_val_compare_and_swap(pointer, outdated_value, updated_value) != outdated_value);
    // while();
    // long long sum = *pointer + value;
    // if (opt_yield)
    //         sched_yield();
    // *pointer = sum;
}

void *add_helper(void * itr) //have to call this first as we need to add 1 and -1 x times which is not a functionality of plain add()
{
    //void (* function)(long long *, long long );
    //function = &add;
    int num_iterations = *((int*)itr);
    int i = 0;
    for(; i < num_iterations; i++)
    {
        if(is_sync)
        {
            switch(sync_option)
            {
                case 'm':
                    add_mutex(&counter, 1);
                    add_mutex(&counter, -1);
                    break;
                case 's':
                    add_sync(&counter, 1);
                    add_sync(&counter, -1);
                    break;
                case 'c':
                    add_compare(&counter, 1);
                    add_compare(&counter, -1);
                    break;
                default:
                    add(&counter, 1);
                    add(&counter, -1);
            }
        }
        else
        {
            add(&counter, 1);
            add(&counter, -1);
        }
    }
    return NULL;
}
void raise_err()
{
    fprintf(stderr, "err: %s", strerror(errno));
    exit(1);
}

void print_err(char * msg)
{
    fprintf(stderr, "err: %s", msg);
    exit(1);
}

int main(int argc, char ** argv)
{
    static struct option long_options[] = {
        {"threads", required_argument, NULL, '1'},
        {"iterations", required_argument, NULL, '2'},
        {"yield", 0, NULL, '3'},
        {"sync", required_argument, NULL, '4'},
        {0,         0,                 NULL,  0 }
    };

    struct timespec start_time, end_time;

    char * short_options = "";
    int option_index = 0;
    int num_threads = 1;
    int num_iterations = 1;
    //char * sync_option = NULL;

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
            case '3':
                opt_yield = 1;
                break;
            case '4':
                if(optarg == NULL)
                {
                    //is_sync = 1;
                    sync_option = 'd';
                    break;
                }
                if((strcmp(optarg, "m") != 0) && (strcmp(optarg, "s") != 0) && (strcmp(optarg, "c") != 0))
                {
                    fprintf(stderr, "unrecognized input! \n");
                    exit(1);
                } 
                else
                {
                    is_sync = 1;
                    sync_option = (char)optarg[0]; //grab the character
                }
                break;
            default:
                fprintf(stderr, "unrecognized input!\n");
                exit(1);
        }
    }
    if(optind < argc)
    {
        fprintf(stderr, "unrecognized input! \n");
        exit(1);
    }
     
     //create thread container
    pthread_t * threads_array = (pthread_t *)malloc(num_threads * sizeof(pthread_t));

    //get start time
    if(clock_gettime(CLOCK_REALTIME, &start_time) < 0)
        raise_err();
    
    
    //create threads
    int i = 0;
    for(; i < num_threads; i++)
    {
        if(pthread_create(&threads_array[i], NULL, add_helper, (void *)&num_iterations))
            print_err("thread error");
    }
    i = 0;
    for(; i < num_threads; i++)
    {
        if(pthread_join(threads_array[i], NULL))
            print_err("thread error");
    } //all threads should be joined now!
    if(clock_gettime(CLOCK_REALTIME, &end_time) < 0)
        raise_err();

    int op = num_threads * num_iterations * 2;
    long long total_time = 1000000000L * (end_time.tv_sec - start_time.tv_sec) + end_time.tv_nsec - start_time.tv_nsec;//end_time.tv_nsec - start_time.tv_nsec;
    int avg_op = (int) total_time/op;

    char * test_name;
    if(opt_yield)
        if(is_sync)
        {
            switch(sync_option)
            {
                case 'm':
                    test_name = "add-yield-m";
                    break;
                case 's':
                    test_name = "add-yield-s";
                    break;
                case 'c':
                    test_name = "add-yield-c";
                    break;
            }
        }
        else
            test_name = "add-yield-none";
    else
        if(is_sync)
        {
            switch(sync_option)
            {
                case 'm':
                    test_name = "add-m";
                    break;
                case 's':
                    test_name = "add-s";
                    break;
                case 'c':
                    test_name = "add-c";
                    break;
            }
        } 
        else
            test_name = "add-none"; 
    printf("%s,%d,%d,%d,%lld,%d,%lld\n", test_name, num_threads, num_iterations, op, total_time, avg_op, counter);

    pthread_exit(NULL);
    return 0;
}