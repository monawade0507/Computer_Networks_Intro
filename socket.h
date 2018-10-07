// ********************************************************
// * A common set of system include files needed for socket() programming
// ********************************************************
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include "log.h"

#define ENDL " (" << __FILE__ << ":" << __LINE__ << ")"

class ClientConnection {
	public:
		void createSocket();
		void setupAddress(std::string ip);
		void bindSocket();
		void listenSocket();
		int  waitingConnection();
		int  processConnection(int connection);

		// UDP Socket Connection functions
		void makeConnection();

	private:
		int sock = -1;						// file descriptor for the server
		int lst  = -1;						// file descriptor for the listener
		struct sockaddr_in
		{
			short     	family;				// address family
			u_short       	port;				// port number
			struct in_addr  sin_addr;			// internet address
			char 		sin_zero[8];
		};
		struct sockaddr_in servAddr;				// struct needed for the server addr
		struct sockaddr_in clientAddr;				// struct needed for the listener addr
		char buffer[256];	
		int portNum = -1;
		int n = 0;
		Log* logger = new Log();
};

