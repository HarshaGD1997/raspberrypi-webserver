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

	return 0;
}
