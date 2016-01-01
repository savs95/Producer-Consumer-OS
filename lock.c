#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>

void producer(int * flag , int * CriticalReg)
{
	pid_t pid = fork(); //Producer Process
	if(pid == 0) //Successful fork
	{
		do //infinite loop
		{
			flag[0] = 1; //telling consumer that it needs it next
			while(flag[1]==1);  //wait for flag as consumer is using it
			CriticalReg[0] = rand()%564; //data buffer is given a value
			printf("Producer data written to buffer: %d\n", CriticalReg[0]); //printing it
			flag[0] = 0; //finished using the data buffer, giving free signal to the consumer
			sleep(1); //to observe output
		} while(1);
	}
	else if (pid < 0) { //error handling
		printf("Failed to create producer process \n");
		exit(1);
	}

}

void consumer(int * flag , int * CriticalReg)
{
	pid_t pid = fork(); //Consumer Process
	if(pid == 0) //Successful fork
	{
		do //infinite loop
		{
			flag[1] = 1; //tell producer that it needs it next
			while(flag[0]==1);  //Wait for as producer is using it
			printf("Consumer data found in buffer: %d\n",CriticalReg[0]); //reading from data buffer
			flag[1] = 0; //finished reading, giving free signal to producer
			sleep(1); //to observe output

		} while(1);
	}
	else if (pid < 0) { //error handling
		printf("Failed to create consumer process \n");
		exit(1);
	}
}

void main()
{
	int Shared1, Shared2 , * flag , * CriticalReg;
	Shared1 = shmget(IPC_PRIVATE, 2*sizeof(int), IPC_CREAT | 0666); //getting shared flag variable
	if (Shared1 < 0) { //error handling
		printf("*** shmget error (server) ***\n");
		exit(1);
	}
	Shared2 = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666); //getting shared buffer
	if (Shared2 < 0) { //error handling
		printf("*** shmget error (server) ***\n");
		exit(1);
	}
	flag = (int *) shmat(Shared1, NULL, 0);       //attaching it to flag
	if (flag == (int *)-1) { //error handling
		printf("*** shmat error (server) ***\n");
		exit(1);
	}
	CriticalReg = (int *) shmat(Shared2, NULL, 0); //attaching it to CriticalReg
	if (CriticalReg == (int *)-1) { //error handling
		printf("*** shmat error (server) ***\n");
		exit(1);
	}
	flag[0] = 0; //free from producer side
	flag[1]= 0; //free from consumer side
	CriticalReg[0] = 0; //initializing critical region
	producer(flag , CriticalReg); //calling producer
	consumer(flag , CriticalReg); //calling consumer
	wait();
}

