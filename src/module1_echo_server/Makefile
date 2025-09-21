
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra

all: echo_server

echo_server: echo_server.cpp
	$(CXX) $(CXXFLAGS) echo_server.cpp -o echo_server

clean:
	rm -f echo_server
