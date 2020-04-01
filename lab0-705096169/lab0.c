/*
NAME: Henry MacArthur
EMAIL: HenryMac16@gmail.com
ID: 705096169
*/

//what is correct handling of unopenable file
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <stdio.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<signal.h> 
#include <stdlib.h>
#include <unistd.h>

void force_segmentation_fault()
{
    char * err = NULL;
    *err = 'H';
    //char create_error = *err;

    //follow nullptr to throw err
}

void signal_handler() //what does the input mean?
{
    //check to see if it is a segmentation fault?
    //printf("%d \n", SIGSEGV);
    //int in = input;
    fprintf(stderr, "segmentation fault occured but caught\n");
    exit(4);
}

int main(int argc, char ** argv)
{

    //printf("asfdsadfa");

    int number_of_arguements = argc;
    int c;
    int option_index = 0;
    char * short_options = "";

    int input = 0;
    int output = 0;
    int segfault = 0;
    int catch = 0;

    char * file;
    char * out;

    int fdI = 0;
    int fdO = 1;

    //void 
    //not sure if I need --
    static struct option long_options[] = {
        {"input", required_argument, NULL, '1'},
        {"output", required_argument, NULL, '2'},
        {"segfault", no_argument, NULL, '3'},
        {"catch", no_argument, NULL, '4'},
        {0,         0,                 NULL,  0 }
    };

    //printf("A");
    while( (c = getopt_long(number_of_arguements, argv, short_options, long_options, &option_index)) != -1)
    {
        //printf("%c \n", c);
        switch(c)
        {
            case '1': //input
                file = optarg;
                input = 1;
                break;
            case '2': //ouput
                out = optarg;
                output = 1;
                break;
            case '3': //segfault
                segfault = 1;
                break;
            case '4': //catch
                catch = 1;
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

    if(input) //is a file
    {
        if(output && (out == file))
        {
            fdI = open(file, O_RDWR); //set input to the file!
        }
        else
        {
            //printf("%s \n", file);
            fdI = open(file, O_RDWR);
        }
        if(fdI < 0)
        {
            fprintf(stderr, "UNABLE TO OPEN INPUT FILE %s \n", strerror(errno));
            exit(2);
        }

        close(0);
        dup(fdI);
        //close(fdI);
    }
    //printf("%s \n", file);
    if(output)
    {
        
        if((input && (out != file)) || !input)
        {
            
            //fdO = creat(out, S_IRWXU);//open(out, O_RDWR | O_CREAT, 00777);//O_RDWR | O_CREAT);
            fdO = open(out, O_RDWR | O_CREAT, S_IRWXU);
        }

        if(fdO < 0)
        {
            fprintf(stderr, "UNABLE TO OPEN OUTPUT FILE %s \n", strerror(errno));
            //return 10;
            exit(3);
        }

        close(1);
        dup(fdO);
        //close(fdO);
    } //otherwise, it means in and out are the same and that is taken care of above
  //loop through and get all arguments
  //all arguments are stored in variables and have flags for each
    if(catch)
    {
        signal(SIGSEGV, signal_handler);
    }

    if(segfault)
    {
        force_segmentation_fault();
        exit(1);
    }
    //copy stdin to stdout
    //need buffer, how much to read, and where to read from

    while(1)
    {
        char buf[1];
        int status = read(0, buf, 1); //read in one character

        if(status == 0)
        {
            break;
        }

        status = write(1, buf, 1);

        
    }
    //close(fdO);
    //close(fdI);
    exit(0);
} 

