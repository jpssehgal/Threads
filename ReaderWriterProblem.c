#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#define CHUNK_SIZE 32
void process_data(char *buffer, int bufferSizeInBytes);
int get_external_data(char *buffer, int bufferSizeInBytes);

sem_t ReaderWriterLock;
sem_t ReaderLock;
sem_t WriterLock;

unsigned int countReader = 0;
char data[] = "0123456789abcdefghijklmnopqrstuvwxyxABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyxABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnoDEFGHIJKLMNOPQRSTUVWXYZ";
/*********************************************************
**********************************************************
***********   DO NOT MODIFY THIS SAMPLE CODE   ***********
**********************************************************
**********************************************************/
int min(int a, int b)
{
        if (a>b)
                return b;
        else
                return a;
}
int get_external_data(char *buffer, int bufferSizeInBytes)
{
    int status;
    int val;
        char srcString[] = "0123456789abcdefghijklmnopqrstuvwxyxABCDEFGHIJKLMNOPQRSTUVWXYZ";

        val = (int)(rand()%min(bufferSizeInBytes, 62));

        if (bufferSizeInBytes < val)
                return (-1);

        strncpy(buffer, srcString, val); // buffer is destination, srcString  is src

        return val;
}

void process_data(char *buffer, int bufferSizeInBytes)
{
    int i;

    if(buffer)
    {
                //printf("thread %i - ", pthread_self());
                for(i=0; i<bufferSizeInBytes; i++)
                {
                        printf("%c", buffer[i]);
                }
                printf("\n");
                memset(buffer, 0, bufferSizeInBytes);
    }
        else
                printf("error in process data \n");

    return;
}
/*********************************************************
**********************************************************
********   PLEASE IMPLEMENT YOUR TEST CODE BELOW   *******
**********************************************************
**********************************************************/

//TODO Define global data structures to be used

/**
 * This thread is responsible for pulling data off of the shared data
 * area and processing it using the process_data() API.
 */
void *reader_thread(void *arg) {
        //TODO: Define set-up required
while(1){
//sleep(1);
        sem_wait (&ReaderWriterLock);           // Lock on the basis of Arrival (to maintain order)
        sem_wait (&ReaderLock);                         // Exclusive lock for all Readers

        if(countReader == 0)
                sem_wait (&WriterLock);                 // If first reader arrives, lock the Writer so that no other Writer can enter the Reader area
        countReader++;                                          // Increment the readers
        sem_post(&ReaderWriterLock);            // Release the first lock, only Reader lock remians now
        sem_post(&ReaderLock);                          // Release the reader lock, Writer lock is STILL enabled so that only readers can access the buffer.

        // Implement code here
        int *reader_threadID = (int*)arg;
        printf("Reader thread [%d] is executing \n", *reader_threadID);
        //while(1)
        //{
                process_data(data, CHUNK_SIZE);
        //}

        sem_wait (&ReaderLock);                         // Lock the Reader so that Reader Count can be maintained.
                countReader--;

        if(countReader == 0)
                sem_post(&WriterLock);                  // If no reader left , then release the Writer lock so that Writer can access the buffer now.
        sem_post(&ReaderLock);
}                               // Release the reader lock, Reading task completed!
        return NULL;
}


/**
 * This thread is responsible for pulling data from a device using
 * the get_external_data() API and placing it into a shared area
 * for later processing by one of the reader threads.
 */
void *writer_thread(void *arg) {
while(1){
//sleep(1);
        sem_wait (&ReaderWriterLock);           //Lock on the basis of Arrival (to maintain order)
        sem_wait (&WriterLock);                         // Exclusive access to writer (one writer at time only!). For Example, W-1 has access now
        sem_post(&ReaderWriterLock);            // Release the first lock, W-1 hold the lock now.

        int *writer_threadID = (int*)arg;
        printf("Writer thread [%d] is executing \n", *writer_threadID);

        get_external_data(data, CHUNK_SIZE);

        sem_wait (&WriterLock);
}                               // W-1 Releases the lock for other Readers or Writers
        return NULL;
}


#define M 10
#define N 20
int main(int argc, char **argv) {
        int i;
        sem_init(&ReaderWriterLock, 0, 1);
        sem_init(&WriterLock, 0, 1);
        sem_init(&ReaderLock, 0, 1);
        pthread_t tid, tidi;
        for(i = 0; i < N; i++) {
                pthread_create(&tid, NULL, reader_thread, (void* )&i);
        }

        for(i = 0; i < M; i++) {
                pthread_create(&tidi, NULL, writer_thread, (void*)&i);
        }


        return 0;
}
