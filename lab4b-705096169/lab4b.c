
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <mraa.h>
#include <math.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <poll.h>
#include <string.h>
#include <time.h>


char temp_scale = 'F';
sig_atomic_t volatile run_flag = 1;

void sample_temp()
{
  printf("ok \n");
}
void on_interrupt()
{
  //printf("hi \n");
  run_flag = 0;
}

float get_temp(float a)
{
  int B = 4275;
  float R = 1023.0/a-1.0;
  int R0 = 100000;
  R = R0*R;

  float temp = 1.0/(log(R/R0)/B+1/298.15)-273.15;
  if(temp_scale == 'F')
    temp = (temp * 9/5) +32;
  return temp;
  
}


mraa_aio_context temp_sensor;
  mraa_gpio_context button;
 
//int num_arg = argc;
//int option_index = 0;


int main(int argc, char ** argv)
{
  int log = 0;
  int log_fd;
  int num_arg = argc;
  int option_index = 0;
  char * short_options = "";

  static struct option long_options[] = {
    {"period", required_argument, NULL, '1'},
    {"scale", required_argument, NULL, '2'},
    {"log", required_argument, NULL, '3'},
    {0, 0, NULL, 0}
  };

  //char * port_name = NULL;
  int period = 1;
  int is_running = 1;
  int cur;
  while((cur = getopt_long(num_arg, argv, short_options, long_options, &option_index)) != -1)
    {
      switch(cur)
	{
	case '1':
	  period = atoi(optarg); //maybe error check?
	  break;
	case '2':
	  if(optarg[0] != 'C' && optarg[0] != 'F')
	    {
	      fprintf(stderr, "invalid scale \n");
	      exit(1);
	    }
	  else
	    temp_scale = optarg[0];
	  break;
	case '3':
	  log_fd = open(optarg, O_RDWR | O_APPEND | O_CREAT | O_TRUNC);
	  if(log_fd < 0)
	    {
	      fprintf(stderr, "error opening file: %s \n", strerror(errno));
	    }
	  log = 1;
	  break;
	default:
	  fprintf(stderr, "input error! \n");
	  exit(1); //have to add the thing at the end
	}
    }
  if(optind < argc)
    {
      fprintf(stderr, "unrecognized input! \n");
      exit(1);
    }
  uint16_t value;
  
   
  button = mraa_gpio_init(60);
  temp_sensor = mraa_aio_init(1);

  if(button == NULL || temp_sensor == NULL)
    {
      fprintf(stderr, "error connecting to sensors");
      exit(1);
    }
  
  mraa_gpio_dir(button, MRAA_GPIO_IN);
  mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &on_interrupt, NULL);
  
  
  //float float_value = mraa_aio_read(temp_sensor);
  //printf("%f \n", get_temp(float_value));
  struct pollfd polls[1] =
    {
      {0, POLLIN | POLLERR | POLLHUP, 0},
    };

  int index = 0;
  char buffer[256];
  char command_buffer[256];

  time_t raw_time;
  struct tm *in;
  time(&raw_time);
  in = localtime(&raw_time);
  //printf("%d:%d:%d \n", in->tm_hour, in->tm_min, in->tm_sec);
  //struct sigaction sample;
  int take_reading = 1;
  int current_time = time(NULL);
  int future_time = current_time + period;
  while(1)
    {
        if(!run_flag)
	{
	  time(&raw_time);
	  in = localtime(&raw_time);
	  value = mraa_aio_read(temp_sensor);

	     char hour[3];
	       char minute[2];
	       char second[2];
	       
	       sprintf(hour, "%d", in->tm_hour);
	       sprintf(minute, "%d", in->tm_min);
	       sprintf(second, "%d", in->tm_sec);
	       //itoa(in->tm_hour, hour, 10);
	       //itoa(in->tm_min, minute, 10);
	       //itoa(in->tm_sec, second, 10);
	       hour[2] = '\0';
	       if(in->tm_hour < 10)
		 {
		   hour[0] = '0';
		   hour[1] = in->tm_hour + '0';
		 }
	       if(in->tm_min < 10)
		 {
		   minute[0] = '0';
		   minute[1] = in->tm_hour + '0';
		 }
	       if(in->tm_sec < 10)
		 {
		   second[0] = '0';
		   second[1] = in->tm_sec + '0';
		 }

	       printf("%s:%s:%s SHUTDOWN\n", hour, minute, second);
	       if(log)
		 dprintf(log_fd,"%s:%s:%s SHUTDOWN\n", hour, minute, second);
	       exit(0);
	       
	}
      int rt_poll = poll(polls, 1, 0);
      if(rt_poll < 0)
	{
	  fprintf(stderr, "error polling: %s", strerror(errno));
	  exit(1);
	}
      else
	{
	  
	  current_time = time(NULL);
	  if(current_time >= future_time)
	    take_reading = 1;

	  //handle input from stdin, use buffer and index
	   if((polls[0].revents&POLLIN) == POLLIN)
	    {
	      //read from standard input, command buffer stores everything, buffer stores what we just read from stdin
	      int status = read(0, &buffer, 256); //get the current input from standard in
	      if(status < 0)
		{
		  fprintf(stderr, "error reading data: %s", strerror(errno));
		}

	      int off = 0;
	      int i = 0;
	      for(; i < status; i++)
		{
		  //look for newline
		  command_buffer[index] = buffer[i];
		  index++;
		  if(buffer[i] == '\n')
		    {
		      		    
		      //this means we have a full command: so see if it is an ok command, parse it, then print it out
		      //ACCEPT: SCALE=F,C PERIOD=seconds, STOP, START, LOG, OFF
		      command_buffer[index] = '\0';
		      
		      if(strcmp("SCALE=C\n\0", command_buffer) == 0)
			{
			  temp_scale = 'C';
			}
		      else if(strcmp("SCALE=F\n\0", command_buffer) == 0)
			{
			  temp_scale = 'F';
			}
		      else if(strcmp("STOP\n\0", command_buffer) == 0)
			{
			  //STOP
			  is_running = 0;
			}
		      else if(strcmp("START\n\0", command_buffer) == 0)
			{
			  is_running = 1;
			  //start
			}
		      else if(command_buffer[0] == 'L' && command_buffer[1] == 'O' && command_buffer[2] == 'G' && command_buffer[3] == ' ')
			{
			  //log command
			  //do nothing?
			}
		      else if(strcmp("OFF\n\0", command_buffer) == 0)
			{
			  if(log)
			    dprintf(log_fd, command_buffer);
			  //handle_exit();
			  //same thing as the off button
			  time(&raw_time);
	  in = localtime(&raw_time);
	  value = mraa_aio_read(temp_sensor);

	     char hour[3];
	       char minute[2];
	       char second[2];
	       
	       sprintf(hour, "%d", in->tm_hour);
	       sprintf(minute, "%d", in->tm_min);
	       sprintf(second, "%d", in->tm_sec);
	       //itoa(in->tm_hour, hour, 10);
	       //itoa(in->tm_min, minute, 10);
	       //itoa(in->tm_sec, second, 10);
	       hour[2] = '\0';
	       if(in->tm_hour < 10)
		 {
		   hour[0] = '0';
		   hour[1] = in->tm_hour + '0';
		 }
	       if(in->tm_min < 10)
		 {
		   minute[0] = '0';
		   minute[1] = in->tm_hour + '0';
		 }
	       if(in->tm_sec < 10)
		 {
		   second[0] = '0';
		   second[1] = in->tm_sec + '0';
		 }

	       printf("%s:%s:%s SHUTDOWN\n", hour, minute, second);
	       if(log)
		 dprintf(log_fd,"%s:%s:%s SHUTDOWN\n", hour, minute, second);
	       exit(0);
			  off = 1;
			}
		      else if(command_buffer[0] == 'P' && command_buffer[1] == 'E' && command_buffer[2] == 'R' && command_buffer[3] == 'I' && command_buffer[4] == 'O' && command_buffer[5] == 'D' && command_buffer[6] == '=')
			{
			  char number[10];
			  int j = 7;
			  int k = 0;
			  while(command_buffer[j] != '\n')
			    {
			      number[k] = command_buffer[j];
			      j++;
			      k++;
			    }
			  number[k] = '\0';
			  //number should now contain the number
			  //make period equal to atoi(number)
			  period = atoi(number);
			}
		      //write(1, &command_buffer, index); //not sure if i write it here?
		      if(log)
			{
			  write(log_fd, &command_buffer, index);
			}
		      index=0;
		      
		      if(off)
			exit(0);
		    }
		}
	    }

	  
	   if(take_reading && (is_running == 1))
	    {
	       time(&raw_time);
	       in = localtime(&raw_time);
	       value = mraa_aio_read(temp_sensor);
	       
	       char hour[3];
	       char minute[3];
	       char second[2];
	      
	       sprintf(hour, "%d", in->tm_hour);
	       hour[2] = '\0';
	       sprintf(minute, "%d", in->tm_min);
	       sprintf(second, "%d", in->tm_sec);
	       minute[2] = '\0';
	       //itoa(in->tm_hour, hour, 10);
	       //itoa(in->tm_min, minute, 10);
	       //itoa(in->tm_sec, second, 10);

	       if(in->tm_hour < 10)
		 {
		   hour[0] = '0';
		   hour[1] = in->tm_hour + '0';
		   //hour[2] = '\0';
		 }
	       if(in->tm_min < 10)
		 {
		   minute[0] = '0';
		   minute[1] = in->tm_hour + '0';
		 }
	       if(in->tm_sec < 10)
		 {
		   second[0] = '0';
		   second[1] = in->tm_sec + '0';
		 }

	       printf("%s:%s:%s %.1f\n", hour, minute, second, get_temp(value));
	       if(log)
		 dprintf(log_fd, "%s:%s:%s %.1f\n", hour, minute, second, get_temp(value));
	       //printf("%d:%d:%d %.1f\n", in->tm_hour, in->tm_min, in->tm_sec, get_temp(value));
	       //printf("take data \n");
	      //take sensor data
	      take_reading = 0;
	      future_time = current_time + period;
	    }
	  //first we are going to handle the button
	  //handle stdin, handle sensor
	 	 	  
	}
    }

  
  mraa_gpio_close(button);
  mraa_aio_close(temp_sensor);
  return 0;
}
