#include "socket.h"

void ClientConnection::createSocket () 
{
	logger->setLogger(true);
	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) 
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

void ClientConnection::setupAddress (std::string ip)
{
	// define the struct
	//srand(time(NULL));
	//portNum = (rand() % 10000 + 1024);
	portNum = 53; 				// DNS is setup over port 53

	// zero the whole struct
	bzero((char *)&servAddr, sizeof(servAddr));

	// Fill in the struct with the information need for the address of the host
	servAddr.family = PF_INET;	
	char* temp = new char[ip.length() + 1];
	strcpy(temp, ip.c_str());
	servAddr.sin_addr.s_addr = inet_addr(temp);
	servAddr.port = htons(portNum);
	if (!inet_aton(temp, &servAddr.sin_addr)) {
		logger->printLog( "inet_aton failded");
		exit(-1);
	}
	logger->printLog("Address has been created for socket");
}




// **************************************************************************************
// **************************************************************************************
// From Project 1 
// **************************************************************************************
// **************************************************************************************
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

// *************************************************************************************
// *************************************************************************************
// End of Project 1
// *************************************************************************************
// *************************************************************************************

void ClientConnection::makeConnection () {
	if (connect(sock, (const sockaddr*) &servAddr, sizeof(servAddr))) {
		logger->printLog ("Connect Failed");
		std::string message = strerror(errno);
		logger->printLog(message);
	}
	logger->printLog ("Connection was made");	
}

void ClientConnection::sendPacket () {
	DugHelp dughelp;
	unsigned char buf[65536];
	if(sendto(sock,(char*)buf, sizeof(dughelp.returnDNSHeader()) + sizeof(dughelp.returnDNSQuestion()), 0, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
		logger->printLog ("sendto Failed");
		std::string message = strerror(errno);
		logger->printLog(message);
	}
	logger->printLog("sendto was successful");
}
