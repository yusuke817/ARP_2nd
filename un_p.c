#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h> 
#include <string.h>
#include <time.h>

#define MAX_SIZE 250000

int size;
int array_size;

int main () 
{
	clock_t t;
	int fd_unnamed[2], ret;
	int child_pid;
	child_pid = fork();
	int data_array[array_size];
	
	bool correct_input = true;
	if (child_pid > 0)
	{
		//I'm in the parent process
		printf("This is the parent process: PID = %d\n", getpid());
		while(correct_input)
		{
			printf("Decide the amount of random Data that will be generated in Mb from 1 to 100: ");
			fflush(stdout);
			scanf("%d", &size);
			if (size <= 0 || size >100)
			{
				printf("\nERROR!! Not the right input! Please choose an integer number between 1 and 100\n\n");
				correct_input = true;
			}
			else
			{
				array_size = size * MAX_SIZE;
				data_array[array_size];
				for (int i=0; i < array_size; i++)
				{
					data_array[i] = rand();
				}
				correct_input = false;
			}
		}	
		close (fd_unnamed[0]);
		t = clock();
		write(fd_unnamed[1], data_array, sizeof(data_array));
		if (ret != sizeof(data_array))
		{
			printf("Write did not return expected value\n");
			exit(2);
		}
		t = clock() - t;
		close(fd_unnamed[1]);
		double time_taken = ((double)t)/CLOCKS_PER_SEC; 
		printf("time taken for transferring data = %f\n", time_taken);
			
	}

	else if (child_pid == 0)
	{
		int read_data_array[array_size];
		//I'm in the child process
		printf("This is the child process: PID = %d\n", getpid());
		printf("Parent process PID = %d\n", getppid());
		close(fd_unnamed[1]);
		ret = (fd_unnamed[0], read_data_array, sizeof(read_data_array));
		close(fd_unnamed[0]);
		if (ret != sizeof(read_data_array))
		{
			printf("Read did not return expected value\n");
			exit(3);
		}
		for (int i = 0; i < array_size; i++)
		{
			printf("read_data_array[%d]=%d", i, read_data_array[i]);
			fflush(stdout);
		}
		sleep(10);
	}
	
	else
	{
		printf("fork() creation failed!!!\n");
	}
	return 0;
}