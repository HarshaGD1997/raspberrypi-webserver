# Basic HTTP server

## Http server which responds back with Hello world

![http flow](assets/http_flow.png)


	- A server listening on port 8080.
	- Accepts connections, reads an HTTP request, and sends back a valid HTTP response.
	- Responds with Hello world to any request.

### Create socket
	int listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	--Creates a TCP/IPv4 socket.

### Bind to port

	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	bind(listen_fd, (sockaddr*)&addr, sizeof(addr));

	-- binds to all interface on port 8080

### Listen for clients
	listen(listen_fd, 10);

	-- informs OS to queue up 10 connections.

### Accept a client
	int client_fd = accept(listen_fd, (sockaddr*)&client, &len);

	--returns a new socket to communicate with the client.

### Read request 
	ssize_t n = read(client_fd, buf, sizeof(buf) - 1);

	--reads raw HTTP request (GET line + headers).

### Parse request line
	std::istringstream iss(request);
	std::string method, path, version;
	iss >> method >> path >> version;

	--extracts HTTP methods (GET), path(/), and version (HTTP/1.0).

### Build HTTP response
	std::string body = "hello world\n";
	response << "HTTP/1.0 200 OK\r\n"
	<< "Content-length : "<<body.size() <<"\r\n"
	<< "Content-type: text/plain\r\n"
	<< "\r\n"
	<< body;

	--returns proper status line, headers, and body.

### Send response and close connection
	write(client_fd, resp.c_str(), resp.size());
	close(client_fd);

