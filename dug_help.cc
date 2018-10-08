#include "dug_help.h"

/*************************************************
Implementation of DNS methods
*/
DugHelp::DugHelp () {
	hostname = "";
	IPaddress = "";
}

void DugHelp::setHostName (std::string name) {
	hostname = name;
}

void DugHelp::setIPaddress (std::string addr) {
	IPaddress = addr;
}

std::string DugHelp::getHostName () {
	return hostname;
}

std::string DugHelp::getIPaddress () {
	return IPaddress;
}

void DugHelp::setQueryType (std::string type) {
	queryType = type;
}

void DugHelp::createQueryHeader () {
	// create Query Header with struct
	dnsHeader = (struct DNS_Header *)&buf;
	dnsHeader->id = (unsigned short) htons(1337);	// 16-bit; some random id that can be checked when a message is recieved
	dnsHeader->flags.rd = 0;			// 1-bit
	dnsHeader->flags.tc = 0;			// 1-bit
	dnsHeader->flags.aa = 0;			// 1-bit
	dnsHeader->flags.opcode = htons(0); 		// 4-bit; standard query = 0; not the QType
	dnsHeader->flags.qr = 0;			// 1-bit; sending a query = 0; recieving a response = 1
	dnsHeader->flags.rcode = htons(0);		// 4-bit
	dnsHeader->flags.z = htons(0);		// 1-bit; must set to 0
	dnsHeader->flags.ra = 0;			// 1-bit
	dnsHeader->gdcount = htons(1); 		// 16-bit; specifies the # of entries in the question section
	dnsHeader->ancount = htons(0);		// 16-bit; specifies the # of resource records in the answer section
	dnsHeader->nscount = htons(0);		// 16-bit; specifies the # of name server resource records in the authority records section
	dnsHeader->arcount = htons(0);		// 16-bit; specifies the # of resource records in the additional records section

}

void DugHelp::stringToHex () {
	// converting hostname to label/data pair using hex representation;
	dnsQuestion->name = (unsigned char*)&buf[255];
	//dnsQuestion->name = (unsigned char*)std::malloc(sizeof(struct DNS_Header));
	hostname.push_back(' ');
	int len = hostname.length();

	std::vector<int> storage;
	std::vector<int> temp;
	int segCount = 0;

	for (int i = 0; i < len; i++)
	{
		if (hostname[i] == '.' || hostname[i] == ' ') {
			storage.push_back(0);
			storage.push_back(segCount);
			for (int j = 0; j < temp.size(); j++){
				storage.push_back(temp[j]);
			}

			// reset
			segCount = 0;
			temp.clear();
			continue;
		}

		char c = hostname[i];
		char C = std::toupper(c);
		temp.push_back((int)C);
		segCount ++;
	}

	for ( int i = 0; i < storage.size(); i++){
		std::cout << storage[i];
		dnsQuestion->name[i] = storage[i];
	}
	std::cout << std::endl;
}

void DugHelp::createQueryQuestion () {
	dnsQuestion = (struct DNS_Question *)&buf;
	stringToHex();
	//dnsQuestion = (struct DNS_Question*)&buf[sizeof((struct DNS_Header*) + strlen((const char*)dnsQuestion->name) + 1)];

	int queryTypeNum = 0;

	if (queryType == "A")     { queryTypeNum = 1;  }
	if (queryType == "NS")    { queryTypeNum = 2;  }
	if (queryType == "CNAME") { queryTypeNum = 5;  }
	if (queryType == "SOA")   { queryTypeNum = 6;  }
	if (queryType == "WKS")   { queryTypeNum = 11; }
	if (queryType == "PTR")   { queryTypeNum = 12; }
	if (queryType == "MX")    { queryTypeNum = 15; }
	if (queryType == "SRV")   { queryTypeNum = 33; }
	if (queryType == "AAAA")  { queryTypeNum = 28; }
	if (queryType == "")	  { queryTypeNum = 1;  }

	dnsQuestion->qdata = (struct DNS_Question_Data *)&buf;
	dnsQuestion->qdata->qtype = htons(queryTypeNum);
	std::cout << "QType value set to: " << queryTypeNum << std::endl;
	dnsQuestion->qdata->qclass = htons(1);
}

void DugHelp::readReceivedBuffer (unsigned char* buffer) {
}

/*************************************************
Implementation of UDP Socket methods
*/

void DugHelp::createSocket ()
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

void DugHelp::setupAddress ()
{

	// define the struct
	//srand(time(NULL));
	//portNum = (rand() % 10000 + 1024);
	portNum = 53; 				// DNS is setup over port 53

	// zero the whole struct
	bzero((char *)&servAddr, sizeof(servAddr));

	// Fill in the struct with the information need for the address of the host
	servAddr.family = PF_INET;
	char* temp = new char[IPaddress.length() + 1];
	strcpy(temp, IPaddress.c_str());
	servAddr.sin_addr.s_addr = inet_addr(temp);
	servAddr.port = htons(portNum);
	if (!inet_aton(temp, &servAddr.sin_addr)) {
		logger->printLog( "inet_aton failded");
		exit(-1);
	}
  	logger->printLog("Address has been created for socket");
}

void DugHelp::makeConnection () {
	if (connect(sock, (const sockaddr*) &servAddr, sizeof(servAddr))) {
		logger->printLog ("Connect Failed");
		std::string message = strerror(errno);
		logger->printLog(message);
		exit(-1);
	}
	logger->printLog ("Connection was made");
}

void DugHelp::sendPacket () {
	/*
	if (sendto(sock, (char*)buf, sizeof(struct DNS_Header) + sizeof(struct DNS_Question), 0, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
		logger->printLog ("sendto Failed");
		std::string message = strerror(errno);
		logger->printLog(message);
		exit(-1);
	}
	logger->printLog ("sendTo was successful");
	*/

	char *message;
        message = (char *)malloc(2048);	
	int totalSize = 0;
	memcpy(message, dnsHeader, sizeof(struct DNS_Header) + 1);
	totalSize += sizeof(struct DNS_Header) + 1;
	memcpy(message+totalSize, dnsQuestion, sizeof(struct DNS_Question) + 1);
        totalSize += sizeof(struct DNS_Question) + 1;

	std::cout << "Send " << totalSize << " amount of bytes" << std::endl;
	std::cout << "Header size: " << sizeof(struct DNS_Header) + 1 << std::endl;
	std::cout << "Question size: " << sizeof(struct DNS_Question) + 1 << std::endl;
	
	int bytesSent = 0;
	if ((bytesSent = write(sock, message, totalSize)) < 0) {
		logger->printLog("error with write(...)");
	}
	std::cout << "write(...) was successful." << std::endl;	

}

int DugHelp::getPacket () {
	while (1) {
		std::memset(buf, 0, sizeof(buf));
		n = 0;
		if ((n = read(sock, buf, 65536)) < 0) {
			logger->printLog("Error reading data");
			exit(-1);
		}
		else {
			logger->printLog("read was successful");
			return -1;
		}
	}

	/*
	if(read(sock, (unsigned char*) buffer, 500) < 0) {
		logger->printLog ("recvfrom Failed");
		std::string message = strerror(errno);
		logger->printLog(message);
		exit(-1);
	}
	else {
		logger->printLog("read was successful");
		// Need to print the results of the read
		for (int i = 0; i < 500; i++) { std::cout << buffer[i]  << std::endl; }
	}
	*/
}
