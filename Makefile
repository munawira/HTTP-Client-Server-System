CC = g++ -Wno-write-strings 
SERVER_FILE = simple_server.cpp
HTTP_SERVER_FILE = http_server.cpp
LOAD_GEN = load_gen.cpp

all: server load_gen

server: $(SERVER_FILE) $(HTTP_SERVER_FILE)
	$(CC) $(SERVER_FILE) $(HTTP_SERVER_FILE) -o server -lpthread
client: $(LOAD_GEN)
	$(CC) $(LOAD_GEN) -o loadgen -lpthread

clean:
	rm -f server loadgen
