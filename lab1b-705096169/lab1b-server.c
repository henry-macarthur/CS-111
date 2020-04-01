/*
    NAME: HENRY MACARTHUR
    EMAIL: HENRYMAC16@GMAIL.COM
    ID: 705096169
*/

#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <poll.h>
#include <signal.h>
#include "zlib.h"

z_stream defstream;
z_stream infstream;
int child_id;
int pipefd1[2];
int pipefd2[2];
int new_fd;
int socket_fd;
int compress_flag = 0;

void exit_prog();
void raise_err();

void sig_pipe_handler(int sig)
{
    if(sig == SIGPIPE)
    {
        exit_prog();
    }
}

void copy_arry (char * a1, char * a2, int s2) //a2 is 
{ //copy into the array
    int i = 0;
    for(; i < s2; i++)
    {
        a1[i] = a2[i];
    }
}


int comp_data(int input_size, int output_size, char * input, char * output)
{
    defstream.zalloc = Z_NULL;
    defstream.zfree = Z_NULL;
    defstream.opaque = Z_NULL;
    deflateInit(&defstream, Z_BEST_COMPRESSION);
    defstream.avail_in = (uInt)input_size;//+1; 
    defstream.next_in = (Bytef *)input; // input char array
    defstream.avail_out = (uInt)output_size;//sizeof(out); 
    defstream.next_out = (Bytef *)(output); 
    
    do
    {
        if(deflate(&defstream, Z_SYNC_FLUSH) < 0)
        {
            exit(0);
        }
    }while(defstream.avail_in > 0);
    //Z_FINISH);
    deflateEnd(&defstream);

    return (int)output_size - defstream.avail_out; //return the size of the new compressed buffer
}

int decomp_data(int input_size, char * comp, char * decomp)
{
    
    infstream.zalloc = Z_NULL;
    infstream.zfree = Z_NULL;
    infstream.opaque = Z_NULL;
    inflateInit(&infstream);
    
    infstream.avail_in = (uInt)input_size;//((char*)defstream.next_out - comp); 
    infstream.next_in = (Bytef *)comp; 
    infstream.avail_out = (uInt)(15); //maybe pass this in 
    infstream.next_out = (Bytef *)decomp; 
     
    //inflateInit(&infstream);
    inflate(&infstream, Z_SYNC_FLUSH);//Z_NO_FLUSH); //maybe set this to flush 
    inflateEnd(&infstream);

    return (int)infstream.total_out;
}

void exit_prog()
{
    

    int exit_information;
    if(waitpid(child_id, &exit_information, 0) < 0)
        raise_err();
    int signal = exit_information & 0x007f;
    int status = WEXITSTATUS(exit_information);

    fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d", signal, status); 
    close(new_fd);
    close(socket_fd);
    exit(0);
}
void raise_err()
{
    fprintf(stderr, "err: %s", strerror(errno));
    exit(1);
}

int read_from_client(int fd)
{
    char c[15];
    int am_rd = read(fd, &c, 15);
    char test_decomp[15];
    if(compress_flag)
    {
        int decomp_size = decomp_data(am_rd, c, test_decomp);
        copy_arry(c, test_decomp, decomp_size);
        am_rd = decomp_size;
        
    }
    if(am_rd < 0)
        close(pipefd2[1]);

    int i = 0;
    for(; i < am_rd; i++)
    { //replace c with test_decomp
        if(c[i] == 0x03)
        {
            if(kill(child_id, SIGINT) < 0) //try adding a handler for this
                raise_err();
            //printf("killing \n");
        }
        else if(c[i] == 0x04) //EOF. as we pass char by char
        {
            close(pipefd2[1]);
        }
        else if(c[i] == '\r' || c[i] == '\n')
        {
            char output[2];
            output[0] = '\r';
            output[1] = '\n';
            write(pipefd2[1], &output[1], 1);
            //write(1, &output[1], 1);
        }
        else
        {
            //write(1, &test_decomp[i], 1);
            if(write(pipefd2[1], &c[i], 1) < 0)
                raise_err();
        }
    }
    return 0;
}

int read_from_shell(int fd)
{
    char c[1000];
    int buffer_size = 1000;
    int am_rd = read(pipefd1[0], c, 300);
    char send_comp[1000];
    if(compress_flag)
    {
        int s_z = comp_data(am_rd, buffer_size, c, send_comp);
        copy_arry(c, send_comp, s_z);
        am_rd = s_z;
    }
    //write(1, send_comp, s_z);

    if(am_rd < 0)
        raise_err();
    if(am_rd == 0)
    { //the shell has been closed
        exit_prog();
    }
    
    if(write(fd, c, am_rd) < 0)
        raise_err();
    if(compress_flag)
        usleep(400); 
        //this sleep makes it so that the client only decompresses
        //one packet at a time. Ensures that server does not send data faster than client can read
    return 0;
}

int main(int argc, char ** argv)
{
    //might want to do error checking to make sure port is valid integer

    char * port_name = NULL;

    int number_of_arguments = argc;
    int option_index = 0;
    char * short_options = "";
    int port_num;

    static struct option long_options[] = {
        {"port", required_argument, NULL, 'p'},
        {"compress", no_argument, NULL, 'c'},
        {0, 0, NULL, 0}
    };

    int cur;
    while( (cur = getopt_long(number_of_arguments, argv, short_options, long_options, &option_index)) != -1)
    {
        switch(cur)
        {
            case 'p':
                port_name = optarg;
                break;
            case 'c':
                //init_comp();
                compress_flag = 1;
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
    if(port_name == NULL)
    {
        fprintf(stderr, "missing the --port argument! \n");
        exit(1);
    }

    struct sockaddr_in serv_addr, client_addr;
    socklen_t client_length;

    port_num = atoi(port_name);

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0)
        raise_err();
    
    bzero((char * ) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_num);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if(bind(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        raise_err();
    
    listen(socket_fd, 5);

    client_length = sizeof(client_addr);
    new_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &(client_length));
    if(new_fd < 0)
        raise_err();
    //write(new_fd, buff, 18);
    //send message to see if we are connected
    //we are connected, fork a new process
    int p1;
    int p2;
    p1 = pipe(pipefd1);
    p2 = pipe(pipefd2);

    if(p1 < 0 || p2 < 0)
    {
        raise_err();
    }
    //parent can send data through fd2, recive through fd1
    //child can send data through fd1, recieve through fd2
    //0 read 1 out
    child_id = fork(); //create child process
    if(child_id < 0)
    {
        raise_err();
    }
    else if(child_id > 0) //parent process
    {
        signal(SIGPIPE, sig_pipe_handler);

        close(pipefd2[0]);
        close(pipefd1[1]);
        
        struct pollfd polls[2] = 
        {
            {new_fd, POLLIN | POLLERR | POLLHUP, 0}, //client
            {pipefd1[0], POLLIN | POLLERR | POLLHUP, 0} //shell
        }; 

        while(1)
        {
            int rt_poll = poll(polls, 2, 0);
            if(rt_poll < 0)
                raise_err();
            else
            {
                if((polls[1].revents&POLLIN) == POLLIN) //shell
                {
                    read_from_shell(new_fd);
                }
                if((polls[0].revents&POLLIN) == POLLIN) //client
                {
                    read_from_client(new_fd);
                }
                if((polls[0].revents&(POLLERR | POLLHUP)))
                {
                    close(pipefd2[1]);
                    //exit_prog();
                }
                if((polls[1].revents&(POLLERR | POLLHUP)))
                {
                    // char idk[100] = "HP encountered!";
                    // write(1, &idk, 15);
                    exit_prog();
                }
            }
        }           
    }
    else //child process
    {
        close(pipefd2[1]);
        close(pipefd1[0]);
        close(0);
        dup(pipefd2[0]);
        close(pipefd2[0]);
        close(1);
        dup(pipefd1[1]);
        dup(pipefd1[1]);
        close(pipefd1[1]);
        char * args[2];
        args[0] = "/bin/bash";
        args[1] = NULL;

        if(execvp(args[0], args) < 0)
            raise_err(); 
    }
    exit(0);
}