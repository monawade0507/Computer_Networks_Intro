#include "socket.h"

void ClientConnection::createSocket () 
{
	logger->setLogger(true);
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		logger->printLog ("Error opening socket");
		exit (-1);
	}
	else 
	{
		logger->printLog("Socket was created");
		logger->printLog("Socket info: " + std::to_string(sock)); 
	}
}

void ClientConnection::setupAddress ()
{
	// define the struct
	srand(time(NULL));
	portNum = (rand() % 10000 + 1024);
	
	// zero the whole struct
	bzero((char *)&servAddr, sizeof(servAddr));

	// Fill in the struct with the information need for the address of the host
	servAddr.family = AF_INET;	
	servAddr.sin_addr.s_addr = INADDR_ANY;
	servAddr.port = htons(portNum);
	logger->printLog("Address has been created for socket");
}

void ClientConnection::bindSocket ()
{
	int bindSuccess = 0;
	int attempts = 0;
	std::string errorString;
	if (bind(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
	{
		errorString = strerror(errno);
		logger->printLog("bind() failed: " + errorString);
		exit(-1);
	}
	else
	{
		logger->printLog("bind() successful");
		logger->printLog("Bind() function returned: " + std::to_string(sock));
		std::cout << "Port number: " << portNum << std::endl;
	}
}

void ClientConnection::listenSocket ()
{
	int listenSocket = 1;
	std::string errorString;
	if (listen(sock, listenSocket) < 0) 
	{
		errorString = strerror(errno);
		logger->printLog("listen() failed: " + errorString);
		exit(-1);
	}
	else
	{
		logger->printLog("listen() successful");
		logger->printLog("listen() fucntion returns: " + std::to_string(sock));
	}
}

int ClientConnection::waitingConnection ()
{
	logger->printLog("Attempting accept()");
	socklen_t sizeClient = sizeof(clientAddr);
	
	lst = accept(sock, (struct sockaddr *) NULL, NULL);
	if (lst < 0)
	{
		std::string errorString = strerror(errno);
		logger->printLog("Error with accept(): " + errorString);			
		exit(-1);
	}
	else
	{
		logger->printLog("accept() successful");
		return processConnection(lst);			// processes all of the strings read into the socket
	}		
}

int ClientConnection::processConnection(int connection)
{
	logger->printLog("Connection made");	
	while(1) {
		std::memset(buffer, 0, sizeof(buffer));
		n = 0;
		if ((n = read(connection, buffer, 255)) < 0) 
		{
			logger->printLog("Error reading data");
			exit(-1);
			return -2;				// error message that indicates read() did not work
		}		
		else
		{
			logger->printLog("read() successful");

			// double for loop created to detect the keyword: QUIT
			int foundQUIT = 0;
			for (int n = 0; n < sizeof(buffer); n++) 
			{
				for (int i = 0; i < std::strlen("QUIT"); i++)
				{
					if ( buffer[n] == "QUIT"[i])
					{
						foundQUIT++;
					}
				}
				// need to find all of the chars that make "QUIT"
				if (foundQUIT >= strlen("QUIT"))
				{
					// assignment says to return 1
					return 1;
				}
			}

			// double for loop created to detect the keyword: CLOSE
			int foundCLOSE = 0;
			for (int n =0; n < sizeof(buffer); n++)
			{
				for (int i = 0; i < std::strlen("CLOSE"); i++)
				{
					if (buffer[n] == "CLOSE"[i])
					{
						foundCLOSE++;
					}
				}
				// need to find all of the chars that make "CLOSE"
				if (foundCLOSE >= strlen("CLOSE"))
				{
					// assignment says to return 0
					return 0;
				}
			}
			n = write(connection, buffer, sizeof(buffer));
			logger->printLog("write() successful");
		}
	}
	// sucessful calls for read() and write(); no keywords detected
	return -1;
}
