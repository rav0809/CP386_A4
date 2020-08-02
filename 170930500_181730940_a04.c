
/*
------------------------------
CP 386 Assignment 4

Submitted by: Ravjot Singh (181730940)
Hassan
------------------------------
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>
#include <semaphore.h> 
#define FILE_NAME "sample4_in.txt"
#define MAX_INPUT_SIZE 256

int *available;     // number available resources  
int **maximum;      // max number of processes 
int **allocated;    // amount allocated per customer
int **cReq;         // customer cReq
int customer;
int resource;
int *sequence;
int safeState;              //safe

int **fileRead(char* readFile);
void *threadRun(void *thread);
int *safetySeq();
void dataOne(int *data, int x);         // single pointer data
void dataMul(int **data, int x, int y); // double pointer dadta

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Less Parameters are entered. Please try again\n");
        return -1;
    }
    resource = argc -1;


    available = malloc(resource+sizeof(int));
    for(int i = 1; i < argc; i++)
    {
        available[i-1] = atoi(argv[i]);
    }

     // read file, assign values

 maximum = fileRead(FILE_NAME);
	 cReq = malloc(customer*sizeof(int *));
    allocated = malloc(customer*sizeof(int *));
	
   
    for (int i = 0; i < customer; i++)
    {
		 cReq[i] = malloc(resource*sizeof(int));
        allocated[i] = malloc(resource*sizeof(int));
       
    }

    safeState = 0; 

    char *uCommand = malloc(sizeof(char) * MAX_INPUT_SIZE);

    printf("Total Number of Customers: %d\n", customer);
    printf("Currently Available Resources: ");
    dataOne(available, resource);
    printf("Maximum Resources from File:\n");
    dataMul(maximum, customer, resource);
	for (int i=0; i<customer; i++){
			for(int j=0; j<resource; j++){
				allocated[i][j] = 0;
				cReq[i][j]=0;
			}
		}

    while(1)
    {
        printf("Enter Command: ");
        fgets(uCommand, MAX_INPUT_SIZE, stdin);

        if(strlen(uCommand) > 0 && uCommand[strlen(uCommand)-1] == '\n')
        {
            uCommand[strlen(uCommand) - 1] = '\0';
        }
		

        if (strstr(uCommand, "RQ"))
        {
            int counter = 0;
            int *array = malloc(sizeof(int) * (resource + 1)); 
            char *token = NULL;
            token = strtok(uCommand, " ");
            while(token != NULL)
            {
                if (counter > 0)
                {
                    array[counter - 1] = atoi(token);
                }
                token = strtok(NULL, " ");
                counter++;
            }

            int c_alloc = array[0]; 
			//printf("Array %d\n",c_alloc);
			//printf("Counter %d\n",counter);
			//printf("Resource %d\n",resource);
            if(c_alloc < customer && counter == resource + 2)
            {
                for(int i = 0; i < resource; i++)
                {
                    allocated[c_alloc][i] = array[i + 1];
					//printf("allocated loop%d\n",allocated[c_alloc][i]);
                    cReq[c_alloc][i] = maximum[c_alloc][i] - allocated[c_alloc][i];
					//printf("Req loop%d\n",cReq[c_alloc][i]);
                    if(cReq[c_alloc][i] < 0)
                    {
                        cReq[c_alloc][i] = 0;
                    }
                }
            }
            else 
            {
                if(c_alloc >= customer)
                {
                    printf("Error with request, Please check value and enter again.\n");
                }
                else
                {
                    printf("Less Parameters Entered, try again.\n");
                }
            }
            free(array);
            sequence = safetySeq();
            printf("Command Implemented\n");

            
            if(sequence[0] == -1)
            {
                safeState = 0;
                printf("State is unsafe.\n");
            }
            else
            {
                safeState = 1;
                printf("State is safe \n");
            }
            
        }
        else if(strstr(uCommand, "RL"))
        {
            int count = 0;
            int *array = malloc(sizeof(int) * (resource + 1));
            char *token = NULL;
            token = strtok(uCommand, " ");
            while(token != NULL)
            {
                if (count > 0)
                {
                    array[count - 1] = atoi(token);
                }
                token = strtok(NULL, " ");
                count++;
            }

            int c_alloc = array[0];
          
            if (c_alloc < customer && count == resource + 2)
            {
                for(int i = 0; i < resource ; i++)
                {
                    if (array[i+1] <= allocated[c_alloc][i])
                    {
                        allocated[c_alloc][i] -= array[i+1];
                        cReq[c_alloc][i] = maximum[c_alloc][i] - allocated[c_alloc][i];
                    }
                    else 
                    {
                        printf("cannot release more resources than allocated.\n");
                        break;
                    }
                }
            }
            else 
            {
                if (c_alloc >= customer)
                {
                    printf("Entry Not correct,Please check values\n");
                }
                else 
                {
                    printf("Less Parameters Entered, try again.\n");
                }
            }
            free(array);
            sequence = safetySeq();
            printf("Request satisfied\n");
            
            if(sequence[0] == -1)
            {
                safeState = 0;
                printf("** State is unsafe Right now.\n");
            }
            else
            {
                safeState = 1;
                printf("Safe State----.\n");
            }
            
        }
        else if (strstr(uCommand, "*"))
        {
            printf("Allocated Resources: \n");
            dataMul(allocated, customer, resource);

            printf("Requested Resuorces: \n");
            dataMul(cReq, customer, resource);

            printf("Available: \n");
            dataOne(available, resource);

            printf("Maximum Resources:\n");
            dataMul(maximum, customer, resource);

        }
        else if(strstr(uCommand, "Run"))
        {
            sequence = safetySeq();
            if (safeState == 1)
            {
                for(int i = 0; i < customer; i++)
                {
                    int runThread = sequence[i];
                    pthread_t tid;
                    pthread_attr_t attr;
                    pthread_attr_init(&attr);
                    pthread_create(&tid, &attr, threadRun, (void *)&runThread);
                    pthread_join(tid, NULL);
                }
            }
            else 
            {
               printf("** Current State is Unsafe.\n");
            }
        }
        else if (strstr(uCommand, "exit"))
        {
            free(available);
            free(maximum);
            free(allocated);
            free(cReq);
            free(sequence);
            return 0;
        }
        else 
        {
            printf("\"%s\" Invalid Input Detected Please use the following commands: [\"RQ\",\"RL\",\"*\",\"Run\",\"exit\"].\n", uCommand);
        }
    }
    return 0; 
}
int **fileRead(char *readFile)
{
    FILE *file = fopen(readFile, "r");
    if (!file)
    {
        printf("Couldn't Open the file to read from\n");
        return NULL;
    }

    struct stat dataStore;
    fstat(fileno(file), &dataStore);
    char *fileContent = (char *)malloc(((int)dataStore.st_size + 1) * sizeof(char));
    fileContent[0] = '\0';
    while(!feof(file))
    {
        char line[100];
        if(fgets(line, 100, file) != NULL)
        {
            strncat(fileContent, line, strlen(line));
        }
    }
    fclose(file);

    char *command = NULL;
    char *fileCopy = (char *) malloc((strlen(fileContent) + 1) * sizeof(char));
    strcpy(fileCopy, fileContent);
    command = strtok(fileCopy, "\r\n");
    while(command != NULL)
    {
        customer++;
        command = strtok(NULL, "\r\n");
    }
    strcpy(fileCopy, fileContent);
    char *lines[customer];
    int i = 0;
    command = strtok(fileCopy, "\r\n");
    while (command != NULL)
    {
        lines[i] = malloc(sizeof(command) * sizeof(char));
        strcpy(lines[i], command);
        i++;
        command = strtok(NULL, "\r\n");
    }
    int **maximum = malloc(sizeof(int *) * customer);
    for (int j = 0; j < customer; j++)
    {
        int *temp = malloc(sizeof(int) * resource);
        char *token = NULL;
        int k = 0;
        token = strtok(lines[j], ",");
        while (token != NULL)
        {
            temp[k] = atoi(token);
            k++;
            token = strtok(NULL, ",");
        }
        maximum[j] = temp;
    }
    return maximum;
}

void *threadRun(void *trh)
{
   int *threadId = (int *)trh; // thread id
   // print what would show after command 'Run'
   printf("--> Customer/Thread %d\n", *threadId);
   
   printf("     Allocated Resources: ");
   // allocaed resource output
   for (int i = 0; i < resource ; i++) {
       printf("%d ", allocated[*threadId][i]);
   } 
   printf("\n");

   printf("     cReqed: ");
   // cReqed output
   for(int i = 0; i < resource; i++) {
       printf("%d ", cReq[*threadId][i]);
   }
    printf("\n");

   printf("     Available: ");
   // avaiable resources outoput
   for (int i = 0; i < resource; i ++){
       printf("%d ", available[i]);
   }
   printf("\n");

   printf("     Thread has started\n");
   sleep(1);

   printf("     Thread has finished\n");
   sleep(1);

   printf("     Thread is releasing resources\n");
   sleep(1);

   printf("     New Available: ");
   // new available resources outout
   for (int i = 0; i < resource; i++){
       available[i] += allocated[*threadId][i];
       printf("%d ", available[i]);
   }
   printf("\n\n");
   sleep(1);

   pthread_exit(NULL);
}

int *safetySeq(){   // function is used to contain safety algroithm that will be used when calling parts of program

    int *done = malloc(customer* sizeof(int) );
    int *sequence = malloc(customer * sizeof(int) );
    int *avail_work = malloc(resource* sizeof(int) );
    for(int i = 0; i < resource; i ++){     
        avail_work[i] = available[i];
		
    }

    int count = 0;
	  int safety = 0;
    while(count < customer){
      
        for (int i = 0; i < customer; i++){
			
            if(done[i] == 0){
                safety = 1;
				//printf("Resource%d \n",resource);
                for (int j = 0; j < resource; j++){
					//printf("REQ%d \n",cReq[i][j]);
					//printf("Avail%d \n",avail_work[j]);
                    if (cReq[i][j] > avail_work[j]){
                        safety = 0;
						//printf("Req: %d\n", cReq[i][j]);
						//printf("Avail: %d\n", avail_work[j]);
                        break;
						
                    }
                }
                if (safety == 1){
					//printf("Hello1");
                    done[i] = 1;
                    sequence[count] = i;
                    count++;
                    safety = 1;

                    for(int j = 0; j < resource; j++){
                        avail_work[j] += allocated[i][j];
                    }
                }
            }
        }
            if(safety == 0){
				//printf("Hello Safety 0");
                for(int k = 0; k < customer; k++){
                    sequence[k] = -1;
					
                }
                free(avail_work);
                free(done);
				//printf("Sequence %d", sequence[0]);
                return sequence;
        }
		else{
			count++;
		}
    }
	
    free(avail_work);
    free(done);
	//printf("Sequence %d", sequence[0]);
    return sequence;
}

void dataOne(int *data, int x)
{
    for (int i = 0; i < x; i++){
        printf("%d", data[i]);
        if ( i < x -1)
            printf(" ");
    }
    printf("\n");
}

void dataMul(int **data, int x, int y)
{
    for (int i = 0; i < x; i ++){
        for (int j = 0; j < y; j ++){
            printf("%d", data[i][j]);
            if(j < y -1)
                printf(" ");
        }
        printf("\n");
    }
}
