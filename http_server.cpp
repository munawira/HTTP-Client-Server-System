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
  vector<string> lines = split(request, '\n');
  vector<string> first_line = split(lines[0], ' ');

  this->HTTP_version = "1.0"; // We'll be using 1.0 irrespective of the request
    /*
   TODO : extract the request method and URL from first_line here
  */
  
  this->method = first_line[0];
  this->url = first_line[1];


//  cout<< "HERE NOW"<<endl;
//      for (vector<string>::iterator t=lines.begin(); t!=lines.end(); ++t) {
//         cout<<*t<<endl;
//     }



  if (this->method != "GET") {
    cerr << "Method '" << this->method << "' not supported" << endl;
    exit(1);
  }
}

HTTP_Response *handle_request(string req) {
  HTTP_Request *request = new HTTP_Request(req);

  HTTP_Response *response = new HTTP_Response();
  std::ifstream inFile;


  string url = string("html_files") + request->url;

  response->HTTP_version = "1.0";

  struct stat sb;
  if (stat(url.c_str(), &sb) == 0) // requested path exists
  {
    response->status_code = "200";
    response->status_text = "OK";
    response->content_type = "text/html";

    string body;

    if (S_ISDIR(sb.st_mode)) {
      /*
      In this case, requested path is a directory.
      TODO : find the index.html file in that directory (modify the url
      accordingly)
      */
     if((stat ("index.html", &sb) == 0)){
      url = url + "index.html";
     }

    }

    /*
    TODO : open the file and read its contents
    */
        
    inFile.open(url); //open the input file

    std::stringstream strStream;
    strStream << inFile.rdbuf(); //read the file
    response->body = strStream.str(); //str holds the content of the file
    std::cout<< "ARE WE HERE" <<endl;
    std::cout << response->body << "\n"; //you can do anything with the string!!!



    /*
    TODO : set the remaining fields of response appropriately
    */
  }
  else {
    response->status_code = "404";

    /*
    TODO : set the remaining fields of response appropriately
    */
  }

  delete request;

  return response;
}

string HTTP_Response::get_string() {
  /*
  TODO : implement this function
  */
// HTTP/1.0 200 OK
// Date: Fri, 31 Dec 1999 23:59:59 GMT
// Content-Type: text/html
// Content-Length: 1354

this->content_type = "text/html";
this->content_length = "30000"; //this->body.size();

string response =  "HTTP/" + this->HTTP_version + " " + this->status_code + " " + this->status_text + "\n" + "Content-Type: " + this->content_type + "\n" +  "Content-Length: " +this->content_length 
+ "\n\n" + this->body + "\n";
cout << response;


 return response;
}