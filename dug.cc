#include "dug.h"

int main(int argc, char *argv[]) {
	DugHelp dug_helper;
	ClientConnection client;

	// Loop through each argument and set it equal to the variable you need
	for (int count = 0; count < argc; count++) {
		if (count == 1) { dug_helper.setHostName(argv[count]);}
		if (count == 2) { dug_helper.setIPaddress(argv[count]);}
	}
	std::cout << "The hostname you have entered: " << dug_helper.getHostName() << std::endl;
	std::cout << "The IP address you have entered: " << dug_helper.getIPaddress() << std::endl;

	// Asking of DNS query type
	std::cout << "What kind of query would you like to perform: " << std::endl;
	std::string userQueryResponse = "";
	std::cin >> userQueryResponse;
	dug_helper.setQueryType (userQueryResponse);
	// create the Query Header
	dug_helper.createQueryHeader();
	// create the Query Question
	dug_helper.createQueryQuestion();
	// create the socket
	client.createSocket();
	// set up socket address
	client.setupAddress(dug_helper.getIPaddress());
	// call connect
	client.makeConnection();


	return 0;
}
