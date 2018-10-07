#include "dug_help.h"

// *************************************************
// Implementation of Class methods and fuctions
// *************************************************


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
	packetHeader.id = htons(1337);			// 16-bit; some random id that can be checked when a message is recieved
	packetHeader.flags.rd = 0;			// 1-bit
	packetHeader.flags.tc = 0;			// 1-bit
	packetHeader.flags.aa = 0;			// 1-bit
	packetHeader.flags.opcode = htons(0); 		// 4-bit; standard query = 0; not the QType
	packetHeader.flags.qr = 0;			// 1-bit; sending a query = 0; recieving a response = 1
	packetHeader.flags.rcode = htons(0);		// 4-bit
	packetHeader.flags.z = htons(0);		// 3-bit; must set to 0
	packetHeader.flags.ra = 0;			// 1-bit
	packetHeader.gdcount = htons(1); 		// 16-bit; specifies the # of entries in the question section 
	packetHeader.ancount = htons(0);		// 16-bit; specifies the # of resource records in the answer section
	packetHeader.nscount = htons(0);		// 16-bit; specifies the # of name server resource records in the authority records section
	packetHeader.arcount = htons(0);		// 16-bit; specifies the # of resource records in the additional records section
	
}

void DugHelp::stringToHex () {
	// converting hostname to label/data pair using hex representation; store result in variable: std::string qname_labelFormat
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
}

void DugHelp::createQueryQuestion () {
	stringToHex();
	int queryTypeNum = 0;

	// create the struct with query question information
	packetQuestion.qname = qname_labelFormat;

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

	packetQuestion.qtype = htons(queryTypeNum);
	std::cout << "QType value set to: " << queryTypeNum << std::endl;
	packetQuestion.qclass = htons(1);
}
