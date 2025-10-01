#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <unistd.h>

#include <cerrno>
#include <iostream>
#include <map>
#include <cstring>
#include <string>
#include <vector>


static bool write_all(int fd, const char* p, size_t n){
	while(n > 0){
		ssize_t w = write(fd, p, n);
		if(w < 0) {
			if(errno == EINTR){
				continue;
			}
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


struct Conn{
	std::string buf;
	bool replied = false;
};

int main(){

	const uint16_t port = 8080;
	signal(SIGPIPE, SIG_IGN);

	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_fd < 0){
		perror("Socket");
		return 1;
	}

	int yes = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

	sockaddr_in addr{};
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	if(bind(listen_fd, (sockaddr*)&addr, sizeof(addr)) < 0){
		perror("Bind");
		return 1;
	}

	if(listen(listen_fd, 128) < 0){
		perror("Listen");
		return 1;
	}

	std::cout << "Select() HTTP/1.0 server on : " << port << std::endl;

	std::map<int, Conn> conns;
	int maxfd = listen_fd;

	while(true){
		fd_set rfds, wfds;
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_SET(listen_fd, &rfds);
		maxfd = std::max(maxfd, listen_fd);

		for(auto& [fd,c] : conns){
			if(!c.replied) {
				FD_SET(fd, &rfds);
			}
			else{
				FD_SET(fd, &wfds);
			}
			if(fd > maxfd) {
				maxfd = fd;
			}
		}

		int ready = select(maxfd + 1, &rfds, &wfds, nullptr, nullptr);
		if(ready < 0) {
			if(errno == EINTR){
				continue;
			}
			perror("select");
			break;
		}

		if(FD_ISSET(listen_fd, &rfds)){
			sockaddr_in client{};
			socklen_t len = sizeof(client);
			int cfd = accept(listen_fd, (sockaddr*)&client, &len);

			if(cfd >= 0){
				conns.emplace(cfd, Conn{});
			}
		}

		std::vector<int> to_close;
		for(auto& [fd,c]: conns){
			if(!c.replied && FD_ISSET(fd, &rfds)){
				char tmp[4096];
				ssize_t n = read(fd, tmp, sizeof(tmp));

				if(n <= 0){
					to_close.push_back(fd);
					continue;
				}

				c.buf.append(tmp, tmp+n);

				if(c.buf.find("\r\n\r\n") != std::string::npos){
					c.replied = true;
				}
			}

			if(c.replied && FD_ISSET(fd, &wfds)){
				if(!send_hello(fd)) {
					to_close.push_back(fd);
					continue;
				}
				to_close.push_back(fd);
			}
		}

		for(int fd : to_close) {
			close(fd);
			conns.erase(fd);
		}

		
		
	}
	close(listen_fd);

	return 0;
}
