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


#include "http_server.hh"

#include <vector>
#include <sys/stat.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>

vector<string> split(const string &s, char delim) {
  vector<string> elems;

  stringstream ss(s);
  string item;

  while (getline(ss, item, delim)) {
    if (!item.empty())
      elems.push_back(item);
  }

  return elems;
}

HTTP_Request::HTTP_Request(string request) {

  //Separate the request into lines
  vector<string> lines = split(request, '\n');
  vector<string> first_line = split(lines[0], ' ');

  this->HTTP_version = "1.0"; // We'll be using 1.0 irrespective of the request
    
    
  //Extract the request method and URL from first_line here
  
  this->method = first_line[0];
  this->url = first_line[1];

  //Only GET method supported for the assignment
  if (this->method != "GET") {
    cerr << "Method '" << this->method << "' not supported" << endl;
    exit(1);
  }
}

//Take request as input and handle all the HTTP processing
HTTP_Response *handle_request(string req) {
\
  HTTP_Request *request = new HTTP_Request(req);

  HTTP_Response *response = new HTTP_Response();
  response->HTTP_version = "1.0";

  std::ifstream inFile;
  char filesize[10];
  string url = string("html_files") + request->url;
  int status = 0;

  struct stat sb;
  struct stat sbfile;

  if (stat(url.c_str(), &sb) == 0) // If requested path exists
  {
    response->status_code = "200";
    response->status_text = "OK";
    response->content_type = "text/html";

    string body;

    //Check if URL is a Directory path
    if (S_ISDIR(sb.st_mode)) {

     url = url + "/index.html";
    }
    //Check if file present in directory
    if((stat (url.c_str(), &sbfile) == 0)){

      status = 1; //FILE FOUND SUCCESSFULLY
      sprintf(filesize, "%jd", sbfile.st_size);
      response->content_length = filesize;

      inFile.open(url); //open the input file

      std::stringstream strStream;
      strStream << inFile.rdbuf(); //read the file
      response->body = strStream.str(); //str holds the content of the file
      
    }
  }
  if(!status){ //FILE NOT FOUND
    
    response->status_code = "404";
    response->status_text = "Not Found";
    response->content_type = "text/html";

    
    string url_404 = string("file_404.html");

    if((stat (url_404.c_str(), &sbfile) == 0)){

      sprintf(filesize, "%jd", sbfile.st_size);
      response->content_length = filesize;
  
      inFile.open(url_404);

      std::stringstream strStream;
      strStream << inFile.rdbuf();
      
      response->body = strStream.str();
    }  
  }
  delete request;
  return response;
}

string HTTP_Response::get_string() {

  string response =  "HTTP/" + this->HTTP_version + " " + this->status_code + " " + this->status_text + "\n" + "Content-Type: " + this->content_type + "\n" +  "Content-Length: " +this->content_length 
  +  "\n\n" + this->body + "\n";

 return response;
}