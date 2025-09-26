#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unistd.h>

#include <cerrno>
#include <iostream>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

static bool write_all(int fd, const char* p, size_t n){
	while(n > 0){
		ssize_t w = ::write(fd, p, n);
		if(w < 0){
			if(errno == EINTR)
				continue;
			return false;
		}

		p += w;
		n -= (size_t)w;
	
	}
	return true;

}

static bool send_hello(int fd){
	std::string body = "hello world\n";
	std::string resp = 
		"HTTP/1.0 200 OK\r\n"
		"Content-Length: " + std::to_string(body.size()) + "\r\n"
		"Content-Type: text/plain\r\n"
		"\r\n" + body;

	return write_all(fd, resp.c_str(), resp.size());


}



