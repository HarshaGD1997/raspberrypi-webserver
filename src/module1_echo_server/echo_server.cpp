#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>


int main(){

	signal(SIGPIPE, SIG_IGN); //POSIX/Linux System call that tells process to ignore the SIGPIPE signal 
	
	const uint16_t port = 8080;
	
	//Create a TCP/IPv4 socket
	
	int listen_fd = ::socket(AF_INET, SOCK_STREAM, 0);

	if(listen_fd < 0) {
		perror("socket");
		return 1;
	}

	//Allow quick restart: SO_REUSEADDR lets us bind while in TIME_WAIT

	int yes = 1;
	if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0){
		perror("setsockopt SO_REUSEADDR");
		return 1;
	
	}

	//Bind to 0.0.0.0:8080 (all interfaces)
	
	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY); //0.0.0.0
	addr.sin_port = htons(port);

	if(bind(listen_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
		perror("bind");
		return 1;
	}

	

	if(listen(listen_fd, 16) < 0) {
		perror("listen");
		return 1;
	}

	std::cout << "Echo server listening on port : "<< port << " ..." << std::endl;



	return 0;
}
