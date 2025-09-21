

---------------------------------------------------------
##    signal (SIGPIPE, SIG_IGN);

	* SIGPIPE is a signal sent to a process when it tries to write to a socket (or pipe) that has been close on the other end.

	* Eg: Server writes data to a client socket. Client has already disconnected or closed the socket. Kernel detects it and sends your process SIGPIPE signal. 
	* A single failed write() on a closed connection could crash your server.


    * signal() is used to set a custom handler for a signal.
    * SIG_IGN means ignore the signal.

    * write() to closed connection returns -1 with errno == EPIPE 


----------------------------------------------------------   

##    const uint16_t port = 8080;

    * creates a fixed, 16-bit non-negative number for server to listen on.
    * Port number remains constant throughout the program lifetime.
    * uint16_t = unsigned 16 bit integer.
    * TCP/UDP port number range from 0 to 65535 (16 bits).
    * uint16_t matched the underlying socket API type (sin_port in sockaddr_in).
    
-------------------------------------------------------------

## int listen_fd = ::socket(AF_INET, SOCK_STREAM, 0);

    * The socket() call is a POSIX systemcall that creates and endpoint for network communication.
    * It returns a file descriptor (an integer) representing the newly created socket (-1 if error occurs).
    * Domain = AF_INET (Uses IPv4 family address like 192.168.1.1).
    * type = SOCK_STREAM ( creates a stream socket, provides reliable, connection-oriented, byte stream communication(TCP).
    * Protocol = 0 (Let OS choose the default protocol for the given domain/type. For AF_INET + SOCK_STREAM this is TCP).

    * :: Ensures the global namespace version of socket() is called.
    * C library function from <sys/socket.h>
    * It avoids accidently shadowing of another class member or socket symbol inside namespace.

    * on Success socket() returns a non-negative integer (e.g., 3), which is file descriptor the kernel used to refer to this socket.
    * on failure it returns a -1, and sets global variable errno to indicate the error (EAFNOSUPPORT, EMFILE,..etc)
    * file descriptor listen_fd will be later used in calls like: bind(), listen(), accept(), send()/recv().

    * error handling : if(listen_fd < 0)  means socket creation failed.
    * perror() prints human readable error message on errno.

------------------------------------------------------------------------

 
## setsockopt()

    int yes = 1;
    if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
        perror("setsockopt SO_REUSEADDR);
        return 1;
    }


    * int yes = 1;
    * create an integer named yes and set the value to 1(true).
    * this will be sent to socket opetion to enable the feature.

    * setsockopt()
    * setsockopt() is a systemcall that sets options on a socket.
        * listen_fd = file descriptor on which we are setting the option.
        * SOL_SOCKET = Specify the option is at socket level.
        * SO_REUSEADDR = the specific option being set.
            *Allows the socket to reuse a local address/port even if it's in the TIME_WAIT state from a previous connection.
            *Without this you might get "Address already in use" if you restart the server quickly.
        * &yes = pointer to the option value (1 to enable).
        * sizeof(yes) = size of the value being passed.

    
    * Error check 
    * if setsockopt returns < 0 it means, setting the option failed.
    * perror("setsockopt SO_REUSEADDR") prints error to the stderr. (Describing why it failed e.g., Insufficient permissions).
    * Exits the program with non-zero status if the option couldn't be set.

    
    * Why SO_REUSEADDR?
    * if TCP connection closes, the port enters a TIME_WAIT state for a few minutes to ensure all packets have cleared.
    * without this option trying to bind() to the same port immediatly will fail.
    * Enabling the SO_REUSEADDR let's you restart a server on the same port without waiting.

    
    * This code enables address reuse on the server socket so the server can be restarted quickly and avoid "Address already in use" errors.

----------------------------------------------------------------------------------

## Setup socket address structure (sockaddr_in). Server can bind to specific IP address and port,

    * sockaddr_in addr{};
    * Declares and zero-initializes a sockaddr_in structure named addr.
    * sockaddr_in is a special struct used to describe an IPv4 address/port combination for network operations like bind().
    * {} ensures all fields are set to 0 initially, preventing garbage values.

    * addr.sin_family = AF_INET;
    * specifies address family 
    * AF_INET = IPv4
    * tells socket function that this is IPv4 socket.

    * addr.sin_addr.s_addr = htonl(INADDR_ANY);
    * Sets IP address on which the server will listen.
    * INADDR_ANY is a constant (0.0.0.0), listen on all available network interfaces (wifi, localhost, ethernet).
    * htonl() converts the 32-bit integer from host byte order to network byte order (big-endian), which is required by network functions.
    * if you wanted to bind specific IP address (e.g., 127.0.0.1), then you'd use inet_pton() to convert a text IP address into binary instead of INADDR_ANY.


    * addr.sin_port = htons(port);
    * set the TCP/UDP port number for socket.
    * port is uint16_t variable (e.g., 8080).
    * htons will convert the 16-bit port number from host-byte order to network byte order.

    * This configuration means: 
        * Protocol family: IPv4
        * IP address: 0.0.0.0 accept connections from any interface.
        * Port: 8080 (or any value the port variable holds).
    * When passed to bind(listen_fd, (sockaddr*)&addr, sizeof(addr)), the server will accept connections on all network interfaces at port 8080.

--------------------------------------------------------------------------------------------------


##   bind() system call

    * bind() is a system call that associates a socket (created earlier with socket()) to a specific address and port.
    * listen_fd = file descritor of the socket you created.
    
    * reinterpret_cast<sockaddr*>(&addr) = a pointer to the address structure (here, an sockaddr_in) cast to the generic type sockaddr* required by the bind() API.

    * sizeof(addr) = size of the address structure.

    * if bind succeds, then the socket is attached to the given IP address and port (e.g., 0.0.0.0:8080).
    * if it fails (returns < 0), perror("bind") prints the reason.

    * About the cast 
    * bind() expects it's second parameter as const struct sockaddr *.
    * addr is declared as struct sockaddr_in (IPv4 specific).
    * reinterpret_cast<sockaddr*>(&addr) simply reinterprets the memory of addr as a generic sockaddr.
    * this is a c++ type cast, safer and more explicit than a c-style (sockaddr*)&addr.

    * bind() function prototype:
        * int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
        * compiler will complain if you pass a sockadd_in* directly where a sockaddr* is expected.

    *reinterpret_cast<> is a c++ type-cast operator that allows you to interpret one pointer type as another without changing underlying bits.
    * nothing about the actual memory changes; it's just a different view of the same bytes.

----------------------------------------------------------------------------------------------------

## listen() 

    * listen(listen_fd, 16) this tells the OS that your socket(listen_fd) should start listening for incoming tcp connection requests.
    * Arguments:
        * listen_fd socket file descriptor that was previously created with socket() and bound to an address with bind().
        * 16 (backlog): backlog is the maximum number of pending connections that can be queued while your program hasn't yet called accept().
        * 16 is the size of the waiting room for clients trying to connect.
        * if more than 16 clients attempt to connect simultaneously, new connection attempts may be refused(depending on the OS).
        * returns 0 on sucess and -1 on the error;

    
    * listen() moves the socket from a passive state(just bound to an address) to a listening state, allowing the kernel to queue incomming connections.
    * backlog (16) here determines how many connection request can wait in line before being accepted.
    * if queue is full, additional connection attemps might fail or be ignored untill space is available.

    * this is the final step before calling accept() to establish actual client connections.

--------------------------------------------------------------------------------------------------------


## Server main loop

    * while(true) 
        * server runs forever, waiting for new clients.
        * each iteration of the loop: waits for a new connection, accepts it, prints the client information.

    * Create a structure to store client info 
    * sockaddr_in client{};
    * socklen_t len = sizeof(client);
    * sockaddr_in client{} : A structure that holds the client IP address and port once a connection is accepted.
    * socklen_t len : A variable to tell accept() how large the client structure is.
    * socklen_t is an integer type specifically used to represent socket address lengths.


    * accepting a new client connection:
        * int client_fd = accept(listen_fd, reinterpret_cast<sockaddr*>(&client), &len);

        * accept() blocks until a client tries to connect to the server's listening socket(listen_fd).     
        * arguments:
            * listen_fd: listen socket created earlier with socket() and listen().
            * reinterpret_cast: safely converts pointer type.
            * &len: on returns, it contains the actual size of the client addresss.

    * return values:
        * client_fd : A socket descriptor dedicated tp communicating with this client.
        * listen_fd : continues listening for other clients.

    
    * handling error: 
        * if(client_fd < 0){
            if(errno == EINTR) continue;
            perror("Accept");
            break;
        }
        
        * if the error is EINTR (interrupted by a signal), retry the loop.
        * otherwise print the error (perror) and break out of the loop.


        * wait for a client tp connect(accpet).
        * retrieve the client's IP/port info.
        * print conection details.
        * loop back to handle more clients.


-------------------------------------------------------------------------------------------------------

## Converting client IP to human-readable string

    * char ipstr[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &client.sin_addr, ipstr, sizeof(ipstr));

    * INET_ADDRSTRLEN = size of buffer needed to store an IPv4 address (e.g., 192.168.1.1).
    * inet_ntop() = converts binary IP address (client.sin_addr) into a readable string and stores it in ipstr.

    * ntohs(client.sin_port) = converts clients port from network byte order to host byte order for readability.

 
-------------------------------------------------------------------------------------------------------------


## Core echo loop


    * It repeatedly reads data from a connected client socket, then writes the same back, until the client disconnects or an error occurs.

    * Create a buffer
        char buf[4096];
    A 4kb buffer to temporarily store the bytes received from the client.


    * Main loop
        while(true) 
            ssize_t n = read(client_fd, buf, sizeof(buf));
    
        * read() tries to receive upto sizeof(buf) bytes from the client socket (client_fd).
        * n is the number of bytes actually read, or an error code.

        if(n == 0) client closed connection. Sends a FIN packet.

        if(n < 0) error occured.

    ssize_t to_write = n, written = 0;
    while(written < to_write) {

        ssize_t w = write(client_fd, buf + written, to_write - written);
        ..
        written += w;
    }    

    * Echo back exactly what was read.
    * loop inside a loop : write() might not send all to_write bytes in one call (called a partial write).
    * inner loop ensures that all bytes read are sent back before reading again.

        
-------------------------------------------------------------------------------------------------------------

## Cleanup 


    done_with_client: 
        close(client_fd);

    close(listen_fd);

--------------------------------------------------------------------------------------------------------------









 
