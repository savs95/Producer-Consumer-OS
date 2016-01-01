#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>

//turn = 0 -> Producer's turn
//turn = 1 -> Consumer's turn

void producer(int * turn , int * CriticalReg)
{
	pid_t pid = fork(); //Producer Process
	if(pid == 0) //Successful fork
	{
		do //infinite loop
		{
			while(turn[0]!=0);  //wait for turn as consumer is using it
			CriticalReg[0] = rand()%564; //data buffer is given a value
			printf("Producer data written to buffer: %d\n", CriticalReg[0]); //printing it
			sleep(1); //to observe output
			turn[0] = 1; //finished using the data buffer, giving turn to the consumer
		} while(1);
	}
	else if (pid < 0) { //error handling
		printf("Failed to create producer process \n");
		exit(1);
	}

}

void consumer(int * turn , int * CriticalReg)
{
	pid_t pid = fork(); //Consumer Process
	if(pid == 0) //Successful fork
	{
		do //infinite loop
		{
			while(turn[0]!=1);  //Wait for as producer is using it
			printf("Consumer data found in buffer: %d\n",CriticalReg[0]); //reading from data buffer
			sleep(1); //to observe output
			turn[0] = 0; //finished reading, giving turn to producer
		} while(1);
	}
	else if (pid < 0) { //error handling
		printf("Failed to create consumer process \n");
		exit(1);
	}
}

void main()
{
	int Shared1, Shared2 , * turn , * CriticalReg;
	Shared1 = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666); //getting shared turn variable
	if (Shared1 < 0) { //error handling
		printf("*** shmget error (server) ***\n");
		exit(1);
	}
	Shared2 = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666); //getting shared buffer
	if (Shared2 < 0) { //error handling
		printf("*** shmget error (server) ***\n");
		exit(1);
	}
	turn = (int *) shmat(Shared1, NULL, 0);       //attaching it to turn
	if (turn == (int *)-1) { //error handling
		printf("*** shmat error (server) ***\n");
		exit(1);
	}
	CriticalReg = (int *) shmat(Shared2, NULL, 0); //attaching it to CriticalReg
	if (CriticalReg == (int *)-1) { //error handling
		printf("*** shmat error (server) ***\n");
		exit(1);
	}
	turn[0] = 0; //starting from producer
	CriticalReg[0] = 0; //initializing critical region
	producer(turn , CriticalReg); //calling producer
	consumer(turn , CriticalReg); //calling consumer
	wait();
}
