
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

all: http_server

http_server: http_server.cpp
	$(CXX) $(CXXFLAGS) http_server.cpp -o http_server

clean:
	rm -f http_server
