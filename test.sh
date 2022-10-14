#!/bin/sh


curl http://127.0.0.1:8090/ > response.html &
curl http://127.0.0.1:8090/apart1 > response1.html &
