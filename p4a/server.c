#include "cs537.h"
#include "request.h"

// 
// server.c: A very, very simple web server
//
// To run:
//  server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

//Global lock and condition variables
pthread_cond_t EMPTY, FILL;
pthread_mutex_t MUTEX;
//declare buffer shared resources as global
int buffers;
int * BUFFER;
int active_size = 0;


//get function implementation
int get (void)
{
    int i=0, connfd;
    //read BUFFER now
    for(i=0; i < buffers; i++)
    {
        if (BUFFER[i] != -1)
        {
            break;
        }
    }
    if (i == buffers)
    {
        printf("\n***** GET FATAL ERROR in child!!! ***** i = %d is equal to buffers = %d ***** I AM TERMINATING\n", i, buffers);
        exit(1);
    }
    connfd = BUFFER[i];
    BUFFER[i] = -1;
    active_size --;
    return connfd;
}

void put (int connfd)
{
    int i=0;
    //write into buffer
    //printf("PUT - connfd is %d\n",connfd);
    for(i=0; i < buffers; i++)
    {
        if (BUFFER[i] == -1)
        {
            break;
        }
    }
    if (i == buffers)
    {
        printf("\n***** PUT FATAL ERROR in child!!! ***** i = %d is equal to buffers = %d ***** I AM TERMINATING\n", i, buffers);
        exit(1);
    }
    BUFFER[i] = connfd;
    active_size ++;
}
// CS537: Parse the new arguments too
void getargs(int *port, int *threads, int *buffers, int argc, char *argv[])
{
    if (argc != 4) {
	fprintf(stderr, "Usage: %s [portnum] [threads] [buffers]\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
    *threads = atoi(argv[2]);
    *buffers = atoi(argv[3]);
}

void * child(void * dummy)
{
    //printf("Hi I am a child \n");
    int connfd;
    while (1)
    {
        pthread_mutex_lock(&MUTEX);
        while(active_size == 0)
        {
            //printf("Lemme sleep please :)\n");
            pthread_cond_wait(&FILL, &MUTEX);
        }
        connfd = get();
        //printf("GET_Apna - connfd is %d\n",connfd);
        pthread_cond_signal(&EMPTY);
        pthread_mutex_unlock (&MUTEX);
        requestHandle(connfd);
        //printf("CHILD: done handling request_0 %d\n", connfd);
	Close(connfd);
       // printf("CHILD: done handling request_1 %d\n", connfd);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen;
    int threads;
    struct sockaddr_in clientaddr;
    int i = 0;

    getargs(&port, &threads, &buffers, argc, argv);

    //create buffer of required size
    BUFFER = (int*) malloc (buffers * sizeof(int));
    if (BUFFER == NULL)
    {
        printf("\n**** ERROR!!! ***** : could not allocate BUFFER of size %d\n", buffers);
        exit(1);
    }
    for (i=0; i<buffers; i++)
    {
        BUFFER[i] = -1;
    }
    //Create threads
    //printf("Done initializing BUFFER entries i %d\n", i);

    // 
    // CS537: Create some threads...
    //
    pthread_t c_threads[threads];
    for (i=0;i<threads;i++)
    {
        pthread_create(&c_threads[i],NULL,&child,NULL);        
    }

    listenfd = Open_listenfd(port);
    while (1) {
	clientlen = sizeof(clientaddr);
        //printf("Got a request\n");
	connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
        
	// 
	// CS537: In general, don't handle the request in the main thread.
	// Save the relevant info in a buffer and have one of the worker threads 
	// do the work.
	// 
	//requestHandle(connfd);
	//Close(connfd);
	pthread_mutex_lock(&MUTEX);
        while(active_size==buffers)
        {
            pthread_cond_wait(&EMPTY,&MUTEX);
        }
        put(connfd);
        pthread_cond_signal(&FILL);
        pthread_mutex_unlock(&MUTEX);
    }

}


    


 
