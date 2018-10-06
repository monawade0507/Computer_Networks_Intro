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

void DugHelp::createQueryHeader () {
	// create Query Header with struct
	dnsheader.id = "AA AAA";	// some random id that can be checked when a message is recieved
	dnsheader.flags.rd = 0;
	dnsheader.flags.tc = 0;
	dnsheader.flags.aa = 0;
	dnsheader.flags.opcode = 0; 	// standard query = 0
	dsnheader.flags.qr = 0;		// sending a query = 0; recieving a response = 1
	dnsheader.flags.rcode = 0;
	dnsheader.flags.z = 0;
	dnsheader.flags.ra = 0;
	dnsheader.gdcount = 1; 		// specifies the # of entries in the question section 
}
