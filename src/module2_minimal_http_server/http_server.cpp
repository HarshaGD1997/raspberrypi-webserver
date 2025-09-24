#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <string>


int main(){

	// Avoid crash
	signal(SIGPIPE,SIG_IGN);

	// Setup listening file descriptor using socket
	const uint16_t port = 8080;
	int listen_fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if(listen_fd < 0){
		
		perror("Socket");

		return 1;
	}

	// Set socket options to reconnect
	int yes = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

	// Bind the socket
	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	
	if(bind(listen_fd, (sockaddr*)&addr, sizeof(addr)) < 0){
		perror("bind");
		return 1;
	}

	if(listen(listen_fd, 10) < 0){
		
		perror("listen");
		return 1;
	}

	std::cout << "HTTP/1.0 server listening on port "<< port << "..." <<std::endl;


	while(true){
		sockaddr_in client{};
		socklen_t len = sizeof(client);
		int client_fd = accept(listen_fd, (sockaddr*)&client, &len);
		if(client_fd < 0){
			perror("accept");
			continue;
		}

		char buf[4096];
		ssize_t n = read(client_fd, buf, sizeof(buf) - 1);
		if(n > 0){
			buf[n] = '\0';
			std::string request(buf);

			// print raw request 
			std::cout << "======== Request ======\n" << request << "===========\n";


			// Parse first line
			std::istringstream iss(request);
			std::string method, path, version;
			iss >> method >> path >> version;



			std::string body = "Hello world\n";
			std::ostringstream response;
			response << "HTTP/1.0 200 OK\r\n"
				<< "Content-length: " <<body.size() << "\r\n"
				<< "Content-type: text/plain\r\n"
				<< "\r\n"
				<< body;

			std::string resp = response.str();
			write(client_fd, resp.c_str(), resp.size());
		}
		close(client_fd);

		}
	
	
	close(listen_fd);


	return 0;
}
