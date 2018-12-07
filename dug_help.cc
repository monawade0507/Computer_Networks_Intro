#include "dug_help.h"

/*************************************************
Implementation of DNS methods
*/
DugHelp::DugHelp () {
	hostname = "";
	IPaddress = "";
}

int DugHelp::hexToDec(char hex[]) {
	int len = strlen(hex);
	len--;
	int base = 1;
	int val = 0;
	long long dec = 0;

	for(int i = 0; hex[i] != '\0'; i++) {
		if (hex[i] >= '0' && hex[i] <= '9') {
			val = hex[i] - 48;
		}
		else if (hex [i] >= 'a' && hex[i] <= 'f') {
			val = hex[i] - 97 + 10;
		}
		dec += val * pow(16, len);
		len--;
	}
	return dec;
}

void DugHelp::setHostName (std::string name) {
	hostname = name;
	//dnsAnswer->name = new unsigned char[name.length()];
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
	memset(buf, 0, sizeof(buf));
	dnsHeader = (struct DNS_Header *)&buf;
	dnsHeader->id = (unsigned short) htons(1337);// 16-bit; some random id that can be checked when a message is recieved
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
	dnsHeader->flags.rd;
}

void DugHelp::stringToDec () {
	// converting hostname to label/data pair using decimal representation;
	hostname.push_back(' ');
	int len = hostname.length();

	std::vector<int> storage;
	std::vector<int> temp;
	int segCount = 0;

	for (int i = 0; i < len; i++)
	{
		if (hostname[i] == '.' || hostname[i] == ' ') {
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

	// storing the dec values found in qname
	qnameSize = storage.size();
	memset(qname, 0, 255);



	for ( int i = 0; i < storage.size(); i++) {
		qname[i] = (unsigned char)storage[i];
	}


 	std::string printOutName = "";
	//testing qname
	for (int i = 0; i < qnameSize; i++){
		printOutName += qname[i];
	}
	logger->printLog ("QName: " + printOutName);

}

void DugHelp::createQueryQuestion () {
	stringToDec();

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
	if (queryType == "")	    { queryTypeNum = 1;  }

	unsigned char questionBuf[qnameSize + 5];
	memset(questionBuf, 0, qnameSize + 4);
	memcpy(questionBuf, qname, qnameSize);
	questionBuf[qnameSize] = 0;
	questionBuf[qnameSize + 1] = 0;
	questionBuf[qnameSize + 2] = queryTypeNum;
	questionBuf[qnameSize + 3] = 0;
	int queryClassNum = 1;
	questionBuf[qnameSize + 4] = queryClassNum;
	memcpy(buf + sizeof(struct DNS_Header), questionBuf, sizeof(questionBuf));
}

/*************************************************
Implementation of UDP Socket methodsbzero((char *)&servAddr, sizeof(servAddr));
*/

void DugHelp::createSocket ()
{
	//logger->setLogger(true);
	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
	{
		logger->printLog ("Error opening socekt");
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

void DugHelp::sendPacket () {
	int bytesSent = sizeof(struct DNS_Header) + qnameSize + 6;
	unsigned char message[bytesSent];
	memset(message, 0, bytesSent);
	memcpy(message, buf, bytesSent);
	logger->printLog("Amount of bytes sent: " + std::to_string(bytesSent));
	if ((sendto(sock, message, bytesSent, 0, (struct sockaddr *)&servAddr, sizeof(servAddr))) < 0) {
		logger->printLog("error with write(...)");
	}
	logger->printLog ("write(...) was successful");
}

int DugHelp::getPacket () {
	memset(buf, 0, 2000);
	if ((n = read(sock, buf, 2000)) < 0) {
			logger->printLog("Error reading data");
			exit(-1);
	}

	 int endPosition = 0;
	 int loc = 0;
	 std::string id = "0x";
	 std::string flags = "0x";
	 std::string question = "0x";
	 std::string ans_RR = "0x";
	 std::string authority_RR = "0x";
	 std::string additional_RR = "0x";
	 std::string query_name = "";
	 std::string zero_char = "";
	 std::string query_type = "0x";
	 std::string query_class = "0x";
	 std::string data = "";
	 std::string ans_type = "0x";
	 std::string ip_addr = "";

	 std::stringstream buffer;
	 std::stringstream temp;
	 // getting the id
	 for (int i = 0; i < 2; i++) {
		 buffer << std::setfill('0') << std::setw(2) << std::hex << (int)buf[endPosition + i];
	 }
	 id += buffer.str();
	 endPosition += 2;

	 buffer.clear();
	 buffer.str(std::string());

	 // get the flags
	 for (int i = 0; i < 2; i++) {
		 buffer << std::setfill('0') << std::setw(2) << std::hex << (int)buf[endPosition + i];
	 }
	 flags += buffer.str();
	 endPosition += 2;

	 buffer.clear();
	 buffer.str(std::string());

	 // get the question code
	 for (int i = 0; i < 2; i++) {
		 buffer << std::setfill('0') << std::setw(2) << std::hex << (int)buf[endPosition + i];
	 }
	 question += buffer.str();
	 endPosition += 2;

	 buffer.clear();
	 buffer.str(std::string());

	 // get the answer RR
	 for (int i = 0; i < 2; i++) {
		buffer << std::setfill('0') << std::setw(2) << std::hex << (int)buf[endPosition + i];
	 }
	 ans_RR += buffer.str();
	 endPosition += 2;

	 buffer.clear();
	 buffer.str(std::string());

	 // get the authority RR
	 for (int i = 0; i < 2; i++) {
		buffer << std::setfill('0') << std::setw(2) << std::hex << (int)buf[endPosition + i];
	}
	authority_RR += buffer.str();
	endPosition += 2;

	buffer.clear();
	buffer.str(std::string());

	// get the additional RR
	for (int i = 0; i < 2; i++) {
		buffer << std::setfill('0') << std::setw(2) << std::hex << (int)buf[endPosition + i];
	}
	additional_RR += buffer.str();
	endPosition += 2;

	buffer.clear();
	buffer.str(std::string());

	// get the query Name
	for (int i = 0; i < qnameSize; i++) {
		buffer << buf[endPosition + i];
	}
	query_name += buffer.str();
	endPosition += qnameSize;

	buffer.clear();
	buffer.str(std::string());

	// There is a zero bit between the query name and the query type
	for (int i = 0; i < 1; i++) {
		buffer << std::setfill('0') << std::setw(2) << std::hex << (int)buf[endPosition + i];
	}
	zero_char += buffer.str();
	endPosition += 1;

	buffer.clear();
	buffer.str(std::string());

	// get the query type
	for (int i = 0; i < 2; i++) {
		buffer << std::setfill('0') << std::setw(2) << std::hex << (int)buf[endPosition + i];
	}
	query_type += buffer.str();
	endPosition += 2;

	buffer.clear();
	buffer.str(std::string());

	// get the query class
	for (int i = 0; i < 2; i++) {
		buffer << std::setfill('0') << std::setw(2) << std::hex << (int)buf[endPosition + i];
	}
	query_class += buffer.str();
	endPosition += 2;

	buffer.clear();
	buffer.str(std::string());

	// Get IP address
	// Step 1: build hex representation to search for Type A : 00 01
	for (int i = 0; i < 150; i++) {
		buffer << std::setfill('0') << std::setw(2) << std::hex << (int)buf[endPosition + i];
	}
	data += buffer.str();

	// Step 2: Look for 00 01
	for (int i = 0; data.length(); i++) {
		if ( (i + 7) < data.length()) {
			if (data[i] == '0' && data[i + 1] == '0' && data[i + 2] == '0' && data[i + 3] == '1' && data[i + 4] == '0' && data[i + 5] == '0' && data[i + 6] == '0' && data[i + 7] == '1' ) {
				temp << std::setfill('0') << std::hex << data[i] << data[i + 1] << data[i + 2] << data[i + 3];
				ans_type += temp.str();
				loc = i;
				break;
			}
		}
	}

	temp.clear();
	temp.str(std::string());
	buffer.clear();
	buffer.str(std::string());

  // Step 3: get IP with the location found
	for (int i = 0; i < 4; i++) {
		temp << std::setfill('0') << std::setw(2) << std::hex << (int)buf[loc + i];
		char hexStr[17];
		temp >> hexStr;
		int num = hexToDec(hexStr);
		buffer << std::to_string(num);
		temp.clear();
		temp.str(std::string());
		//buffer << (int)buf[loc + i];
		if (i < 3) { buffer << ".";}
	}
	ip_addr += buffer.str();

	 // Printing out Answers
	 logger->printLog("ID: " + id);
	 logger->printLog("Flags: " + flags);
	 logger->printLog("Question code: " + question);
	 logger->printLog("Answer RR: " + ans_RR);
	 logger->printLog("Authority RR: " + authority_RR);
	 logger->printLog("Additonal RR: " + additional_RR);
	 logger->printLog("Query Name: " + query_name);
	 logger->printLog("Query Type: " + query_type);
	 logger->printLog("Query Class: " + query_class);
	 logger->printLog("Answer Type: " + ans_type);
	 if (ans_type == "0x0001") {
		 std::cout << "Authoritative Answer: " << ip_addr << std::endl;
	 }

}
