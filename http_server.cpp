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

  cout<<"Reached http request" << endl;
  cout <<request <<endl;
  vector<string> lines = split(request, '\n');
  vector<string> first_line = split(lines[0], ' ');

  this->HTTP_version = "1.0"; // We'll be using 1.0 irrespective of the request
    
    /*
   TODO : extract the request method and URL from first_line here
  */
  cout<<"Reached http request" << endl;
  cout <<request <<endl;
  this->method = first_line[0];
  this->url = first_line[1];


  if (this->method != "GET") {
    cerr << "Method '" << this->method << "' not supported" << endl;
    exit(1);
  }
}

HTTP_Response *handle_request(string req) {

  cout<<"Request string for HTTP_Request :" << req << endl;
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

    if (S_ISDIR(sb.st_mode)) {
      /*
      In this case, requested path is a directory.
      TODO : find the index.html file in that directory (modify the url
      accordingly)
      */

     url = url + "/index.html";
    }

    //TODO: Check with ashwin about parsing empty directory without html files

    
    if((stat (url.c_str(), &sbfile) == 0)){

      status = 1;
      sprintf(filesize, "%jd", sbfile.st_size);
      response->content_length = filesize;
      cout << "Content Length :" << response->content_length << endl;

      /*TODO : open the file and read its contents
      */    
      inFile.open(url); //open the input file

      std::stringstream strStream;
      strStream << inFile.rdbuf(); //read the file
    
      /*
      TODO : set the remaining fields of response appropriately
      */
     
      response->body = strStream.str(); //str holds the content of the file
      
      std::cout<< "Response for File Found:" <<endl;
      std::cout << response->body << "\n"; //you can do anything with the string!!!
    } 
  }
  if(!status){
    
    //struct stat sb_404;
    response->status_code = "404";
    response->status_text = "Not Found";
    response->content_type = "text/html";

    
    string url_404 = string("/file_404.html");

    if((stat (url_404.c_str(), &sbfile) == 0)){

      sprintf(filesize, "%jd", sbfile.st_size);
      response->content_length = filesize;
      cout << "Content Length :" << response->content_length << endl;
  
      inFile.open(url_404);

      std::stringstream strStream;
      strStream << inFile.rdbuf();
      
      response->body = strStream.str();
      std::cout<< "Response for URL not found" <<endl;
      std::cout << response->body << "\n"; 
    }  

  }

  delete request;

  return response;
}

string HTTP_Response::get_string() {
  /*
  TODO : implement this function
  */


//Content length initialized earlier;
//TODO: IMPLEMENT TIME

string response =  "HTTP/" + this->HTTP_version + " " + this->status_code + " " + this->status_text + "\n" + "Content-Type: " + this->content_type + "\n" +  "Content-Length: " +this->content_length 
+  "\n\n" + this->body + "\n";
cout << response;


 return response;
}