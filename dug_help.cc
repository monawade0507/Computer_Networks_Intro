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

