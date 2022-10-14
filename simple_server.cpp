/* run using ./server <port> */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netinet/in.h>

#include <pthread.h>
#include "http_server.hh"

#define MAX_WORKER_THREADS 10
#define MAX_CONNECTIONS 10

int num_connections =0;

//NEED TO CHECK BELOW
int no_sockfd = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;



//Shared queue of Socket File Descriptor
queue<int> threadsockfd;

void error(char *msg) {
  perror(msg);
  //exit(1);//TODO: check if commenting is required
}

//Function called for execution by worker threads
void *worker_function(void *) {
  
  int my_sockfd,status; //Collect my_sockfd from the queue

  while(1){
    pthread_mutex_lock(&mutex);
    while (num_connections == 0)
      pthread_cond_wait(&empty, &mutex);

    my_sockfd = threadsockfd.front();
    threadsockfd.pop();
    num_connections--;
    pthread_cond_signal(&full);
    pthread_mutex_unlock(&mutex);

    string response_buffer;
    char send_buffer[8000];
    // = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\n";
    HTTP_Response *response;

    // ...thread processing...
    char  receive_buffer[8000];

    printf("In thread: %d \n",my_sockfd );

    bzero(receive_buffer, 8000);
    status = read(my_sockfd, receive_buffer, 30000); 

    if (status < 0)
      error("ERROR reading from socket\n");
    //printf("Here is the message: %s", receive_buffer);

    
    //Process Received Buffer and create the send buffer

    response = handle_request(receive_buffer);

    // Collect response buffer to send to client
    response_buffer = response->get_string();

    //strcat(send_buffer, "Hello World ");
    //cout<<"SendBuffer C String:  " <<endl << response_buffer.c_str();
    bzero(send_buffer, 8000);
    strcat(send_buffer,response_buffer.c_str());

    /* send reply to client */
    status = write(my_sockfd, send_buffer, 8000);
    if (status < 0)
      error("ERROR writing to socket\n");

    close(my_sockfd); 
    cout << "Closed Socket" << endl;
  }

return 0;

}

int main(int argc, char *argv[]) {
  int sockfd,portno;
  
  int newsockfd;
  socklen_t clilen;
  char buffer[256];
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

    // When you create a worker thread to handle a client request at your server, 
    // you must pass the accepted client file descriptor as an argument to the thread function, so that it can read and write from the assigned client. 
    // Understand how arguments are passed to threads, and be careful with pointers and casting.

    /* accept a new request, create a newsockfd */
    printf("Current Thread Count is : %d\n", thread_count);

    pthread_mutex_lock(&mutex);
    while(threadsockfd.size() >= MAX_CONNECTIONS)
        pthread_cond_wait(&full, &mutex);
    pthread_mutex_unlock(&mutex);    

    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
      printf("ERROR on accept\n");

  //Add Sock Fd to queue and signal workers
    pthread_mutex_lock(&mutex);
    threadsockfd.push(newsockfd);
    num_connections++;
    pthread_cond_signal(&empty);
    pthread_mutex_unlock(&mutex);
    printf("new socket created \n"); 

  }


  return 0;
}