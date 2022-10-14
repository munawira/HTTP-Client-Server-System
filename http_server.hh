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

#ifndef _HTTP_SERVER_HH_
#define _HTTP_SERVER_HH_

#include <iostream>
#include <queue>

using namespace std;



struct HTTP_Request {
  string HTTP_version;

  string method;
  string url;

  // TODO : Add more fields if and when needed

  HTTP_Request(string request); // Constructor
};

struct HTTP_Response {
  string HTTP_version; // 1.0 for this assignment

  string status_code; // ex: 200, 404, etc.
  string status_text; // ex: OK, Not Found, etc.
  string response_date;
  
  string content_type;
  string content_length;


  string body;

  // TODO : Add more fields if and when needed

  string get_string(); // Returns the string representation of the HTTP Response
};

HTTP_Response *handle_request(string request);

#endif  
