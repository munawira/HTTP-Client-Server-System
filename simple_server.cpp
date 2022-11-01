////////////////////////////////////////////////////////////////////////////////////
// Author: Munawira Kotyad, 22DO372, munawira@cse.iitb.ac.in
//
// Program: HTTP Server Thread Pool for PA3
// Course: Design and Engineering of Computing Systems at IIT Bombay, August 2022
// 
// The following code implements: 
// 1. HTTP Server with a worker thread pool
//
////////////////////////////////////////////////////////////////////////////////////

/* run using ./server <port> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>

#include <pthread.h>
#include "http_server.hh"

#define MAX_WORKER_THREADS 500
#define MAX_CONNECTIONS 500

int num_connections =0;

//NEED TO CHECK BELOW
int no_sockfd = 0;
pthread_mutex_t q_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t q_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t q_full = PTHREAD_COND_INITIALIZER;

//Shared queue of Socket File Descriptor
queue<int> threadsockfd;

void error(char *msg) {
  perror(msg);
}

//Function called for execution by worker threads
void *worker_function(void *) {
  
  int my_sockfd,status; 

  while(1){

    //Wait on master thread's signal until queue is empty
    pthread_mutex_lock(&q_mutex);

    while (num_connections == 0)
      pthread_cond_wait(&q_empty, &q_mutex);
    my_sockfd = threadsockfd.front();//Collect my_sockfd from the queue
    threadsockfd.pop();
    num_connections--;
    pthread_cond_signal(&q_full);//Signal to the master in case the master is waiting on a full queue. 
    
    pthread_mutex_unlock(&q_mutex);

    //Data structures for sending response
    string response_buffer;
    char send_buffer[8000];
    HTTP_Response *response;

    // ...thread processing...
    char  receive_buffer[8000];
    bzero(receive_buffer, 8000);

    //Read for Client
    status = read(my_sockfd, receive_buffer, 30000); 
    if (status < 0)
      error("ERROR reading from socket\n");

    //Process Received Buffer and create the response
    response = handle_request(receive_buffer);

    // Collect response buffer to send to client
    response_buffer = response->get_string();
    bzero(send_buffer, 8000);
    strcat(send_buffer,response_buffer.c_str());

    //cout << "SEND BUFFER: " << endl << send_buffer << endl;

    /* send reply to client */
    status = write(my_sockfd, send_buffer, 8000);
    if (status < 0)
      error("ERROR writing to socket\n");

    sleep(2);  
    close(my_sockfd); 
  }
return 0;
}

int main(int argc, char *argv[]) {
  int sockfd,portno;
  
  int newsockfd;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;
  pthread_t thread_id[MAX_WORKER_THREADS];
  int thread_count =0;
  int n;


  if (argc < 2) {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(1);
  }

  //CREATE WORKER THREADS
  for (size_t i = 0; i < MAX_WORKER_THREADS; i++){
    /* code */
    pthread_create(&thread_id[thread_count], NULL, worker_function, NULL);
    thread_count++;

  }
  
  /* create server socket */

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

    //Assign new connection requests to worker threads

    /* accept a new request, create a newsockfd */

  //Wait if the queue is at maximum connections
    pthread_mutex_lock(&q_mutex);
    while(threadsockfd.size() >= MAX_CONNECTIONS)
        pthread_cond_wait(&q_full, &q_mutex);
    pthread_mutex_unlock(&q_mutex);    

    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0){
      printf("ERROR on accept\n");
      continue;
    }
      

  //Add Sock Fd to queue and signal workers waiting on empty queue
    pthread_mutex_lock(&q_mutex);
    threadsockfd.push(newsockfd);
    num_connections++;
    pthread_cond_signal(&q_empty);
    pthread_mutex_unlock(&q_mutex);


  }
  return 0;
}