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
	packetHeader.id = (unsigned short) htons(1337);	// 16-bit; some random id that can be checked when a message is recieved
	packetHeader.flags.rd = 0;			// 1-bit
	packetHeader.flags.tc = 0;			// 1-bit
	packetHeader.flags.aa = 0;			// 1-bit
	packetHeader.flags.opcode = htons(0); 		// 4-bit; standard query = 0; not the QType
	packetHeader.flags.qr = 0;			// 1-bit; sending a query = 0; recieving a response = 1
	packetHeader.flags.rcode = htons(0);		// 4-bit
	packetHeader.flags.z = htons(0);		// 1-bit; must set to 0
	packetHeader.flags.ra = 0;			// 1-bit
	packetHeader.gdcount = htons(1); 		// 16-bit; specifies the # of entries in the question section
	packetHeader.ancount = htons(0);		// 16-bit; specifies the # of resource records in the answer section
	packetHeader.nscount = htons(0);		// 16-bit; specifies the # of name server resource records in the authority records section
	packetHeader.arcount = htons(0);		// 16-bit; specifies the # of resource records in the additional records section

}

void DugHelp::stringToHex () {
	// converting hostname to label/data pair using hex representation;
	// store result in variable: std::string qname_labelFormat
	static const char* const hex = "0123456789ABCDEF";
	hostname.push_back(' ');
	int len = hostname.length();

	int segCount = 0;
	std::vector<std::string> storage;
	std::string temp = "";

	for (int i  = 0; i < len; i++) {
		if (hostname[i] == '.' || hostname[i] == ' ') {
			storage.push_back(std::to_string(0));
			storage.push_back(std::to_string(segCount));
			storage.push_back(temp);
			// reset
			segCount = 0;
			temp = "";
			continue;
		}
		const unsigned char c = hostname[i];
		std::cout << c << " : " << hex[c >> 4] << " " << hex[c & 15] << std::endl;
		temp.push_back(hex[c >> 4]);
		temp.push_back(hex[c & 15]);
		segCount ++;
		continue;
	}

	storage.push_back(std::to_string(0));
	storage.push_back(std::to_string(0));

	for (int i = 0; i < storage.size(); i++) { qname_labelFormat.append(storage[i]); }

	std::cout << "The hex representation of qname TOTAL: " << qname_labelFormat << std::endl;

	packetQuestion.name = {0};
	packetQuestion.name = (unsigned char*)qname_labelFormat.c_str();
}

void DugHelp::createQueryQuestion () {
	stringToHex();
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

	packetQuestion.qdata->qtype = htons(queryTypeNum);
	std::cout << "QType value set to: " << queryTypeNum << std::endl;
	packetQuestion.qdata->qclass = htons(1);
}

void DugHelp::readReceivedBuffer (unsigned char* buffer) {
	struct DNS_Header *dns = NULL;
	dns = (struct DNS_Header*)buffer;
	for ( int i = 0; i < 20; i++) {
	std::cout << ntohs(buffer[i]) << std::endl; }
	std::cout << "Answers: " << ntohs(dns->flags.rcode) << std::endl;
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
	// find the size to allocate for the message buffer
	int size = sizeof(struct DNS_Header);
	size += sizeof(struct DNS_Question);
	char *message[size];
	int totalSize = 0;

	std::memcpy(message, (struct DNS_Header*)&packetHeader, sizeof(packetHeader));
	totalSize = sizeof(struct DNS_Header);

	std::memcpy(message + totalSize, (struct DNS_Question*)&packetQuestion, sizeof(packetQuestion));
	totalSize += sizeof(packetQuestion);

	int bytesSent = 0;
	if ((bytesSent = write(sock, message, totalSize)) < 0) {
		logger->printLog ("write Failed");
		std::string error = strerror(errno);
		logger->printLog(error);
	}
	else {
		logger->printLog ("write was successful");
	}
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
			for (int i = 0; i < 30; i++) { std::cout << buffer[i] << std::endl;			}
			return 1;
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
