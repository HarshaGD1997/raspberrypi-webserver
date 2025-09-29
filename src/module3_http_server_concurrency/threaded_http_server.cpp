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

static void handle_client(int client_fd){

	char buf[4096];
	ssize_t n = ::read(client_fd, buf, sizeof(buf));
	if(n <= 0){
		::close(client_fd);
		return;
	}
	
	send_hello(client_fd);
	::close(client_fd);	

}


int main(){


	::signal(SIGPIPE, SIG_IGN);
	const uint16_t port = 8080;
	int listen_fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if(listen_fd < 0){
		perror("socket");
		return 1;
	}

	int yes = 1;
	::setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));


	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	
	if(::bind(listen_fd, (sockaddr*)&addr, sizeof(addr)) < 0){
		perror("bind");
		return 1;
	}

	if(::listen(listen_fd, 128) < 0){
		perror("listen");
		return 1;
	}

	std::cout << "Threaded HTTP/1.0 server on : " << port << std::endl;


	while(true){
		sockaddr_in client{};
		socklen_t len = sizeof(client);
		int cfd = ::accept(listen_fd, (sockaddr*)&client, &len);
		if(cfd < 0){
			if(errno == EINTR)
				continue;
			perror("accept");
			continue;
		}

		std::thread(handle_client, cfd).detach();
	}


	return 0;
}



