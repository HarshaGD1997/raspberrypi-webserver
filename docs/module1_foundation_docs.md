

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

 

 
