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


	//accept loop (blocking) one client at a time.
	
	while(true) {
		
		sockaddr_in client{};	
		socklen_t len = sizeof(client);
		int client_fd = accept(listen_fd, reinterpret_cast<sockaddr*>(&client), &len);
		if(client_fd < 0){
			if(errno == EINTR) 
				continue; //Interrupted by signal, retry
			perror("accept");
			break;
		}
		
		char ipstr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &client.sin_addr, ipstr, sizeof(ipstr));
		std::cout << "Client connected from " <<ipstr << ":" << ntohs(client.sin_port) << std::endl;


		//echo loop: read -> write_back -> repeat until client closes.	
		
		char buf[4096];
		while(true) {
			ssize_t n = read(client_fd, buf, sizeof(buf));

			if(n == 0){
				std::cout << "Client closed connection" << std::endl;
				break;
			
			}

			if(n < 0){
				if(errno == EINTR) 
					continue;
				perror("read");
				break;
			
			}

			//Write the same bytes back
			
			ssize_t to_write = n, written = 0;
			while (written < to_write) {
				ssize_t w = write(client_fd, buf + written, to_write - written);
				if (w < 0){
					if(errno == EINTR)
						continue;
					perror("write");
					goto done_with_client;
				}

				written += w;
			
			}
		
		
		}

		done_with_client:
			close(client_fd);

	}

	close(listen_fd);
	return 0;
}
