/* run using ./server <port> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>

#include <pthread.h>
#include "http_server.hh"

#define TOTAL_THREADS 100

void error(char *msg) {
  perror(msg);
  //exit(1);//TODO: check if commenting is required
}

//Function called for execution by worker threads
void *worker_function(void *arg) {
  int my_sockfd = *((int *) arg);
  int n;

  // ...thread processing...
  char  worker_buffer[256];
  printf("In thread: %d \n",my_sockfd );

  while(1){
    
    bzero(worker_buffer, 256);
    n = read(my_sockfd, worker_buffer, 255); 
    if (n < 0)
      error("ERROR reading from socket\n");
    printf("Here is the message: %s", worker_buffer);

    //Handle Client closed connection
    if(n == 0){
      error("Client closed connection\n");  
      close(my_sockfd);
      pthread_exit(NULL);
      break;
    }

    /* send reply to client */
    n = write(my_sockfd, "I got your message", 18);
    if (n < 0)
      error("ERROR writing to socket\n");
      
  }


}

int main(int argc, char *argv[]) {
  int sockfd, newsockfd[TOTAL_THREADS], portno;
  socklen_t clilen;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr[TOTAL_THREADS];
  pthread_t thread_id[TOTAL_THREADS];
  int thread_count =0;
  int n;


  if (argc < 2) {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
  }

  /* create socket */

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");

  /* fill in port number to listen on. IP address can be anything (INADDR_ANY)
   */

  bzero((char *)&serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* bind socket to this port number on this machine */

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");

  /* listen for incoming connection requests */

  listen(sockfd, 5);
  clilen = sizeof(cli_addr);

  while(1){

    // When you create a worker thread to handle a client request at your server, 
    // you must pass the accepted client file descriptor as an argument to the thread function, so that it can read and write from the assigned client. 
    // Understand how arguments are passed to threads, and be careful with pointers and casting.

    /* accept a new request, create a newsockfd */
    printf("Current Thread Count is : %d\n", thread_count);

    newsockfd[thread_count] = accept(sockfd, (struct sockaddr *)&cli_addr[thread_count], &clilen);
    if (newsockfd[thread_count] < 0)
      printf("ERROR on accept\n");

    // Spawn a new thread for each connection

    pthread_create(&thread_id[thread_count], NULL, worker_function, &newsockfd[thread_count]);
    
    printf("New Thread Created\n");
    thread_count++;

  }


  return 0;
}