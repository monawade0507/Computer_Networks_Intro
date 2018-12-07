#include "dug.h"

static void daemonize(DugHelp dug_helper)
{
	pid_t pid = 0;
	int fd;

	// Fork off the parent process
	pid = fork();

	// Check to see if an error occured
	if (pid < 0) {
		dug_helper.log("Failure forking the first process");
		exit(-1);
	}
	// If successful, let the parent terminate
	else {
		dug_helper.log("Success forking first process");
		exit(0);
	}

	// After success, the child process becomes the session leader
	// obtain a new process group
	if (setsid() < 0) {
		dug_helper.log("Failure with setsid() function");
	}

	// close all descriptor
	for (fd = getdtablesize(); fd >= 0; --fd) { close(fd); }
	fd = open("/dev/null", O_RDWR); // open stdin
	dup(fd);  // stdout
	dup(fd);	//stderr

	// ignore signal sent from child to parent process
	signal(SIGCHLD, SIG_IGN);

	// Fork off for the second time
	pid = fork();

	// check if an error occured
	if (pid < 0) {
		dug_helper.log("Failure forking the second process");
		exit(-1);
	}
	else {
		dug_helper.log("Sucessful forking the second process");
		exit(0);
	}

	std::cout << "Current Process ID: " << getpid() << std::endl;
}


int main(int argc, char *argv[]) {
	DugHelp dug_helper;
	std::string userQueryResponse = "A";
	int opt = 0;
	bool _daemonize = false;

	// Loop through each argument and set it equal to the variable you need
	for (int count = 0; count < argc; count++) {
		if (count == 1) { dug_helper.setHostName(argv[count]);}
		if (count == 2) { dug_helper.setIPaddress(argv[count]);}
	}

	while ((opt = getopt(argc, argv, "vtf")) != -1) {
		// v = verbose
		// t = type of query
		// f = daemon
		switch(opt) {
			case 'v':
				dug_helper.turnOnLogger();
				dug_helper.log("Verbose Action captured");
				break;
			case 't':
				userQueryResponse = argv[argc - 1];
				dug_helper.log("User Query captured");
				break;
			case 'f':
				dug_helper.log("Daemon flag captured");
				_daemonize = true;
				break;
			case ':':
			case '?':
			default:
				exit(-1);
		}
	}
	dug_helper.log("The hostname you have entered: " + dug_helper.getHostName());
	dug_helper.log("The IP address you have entered: " + dug_helper.getIPaddress());
	dug_helper.log("User Query: " + userQueryResponse);

	if (_daemonize) {
			daemonize(dug_helper);
			// may not need the while loop
			while(1) {
				dug_helper.setQueryType (userQueryResponse);
				// create the Query Header
				dug_helper.createQueryHeader();
				// create the Query Question
				dug_helper.createQueryQuestion();
				// create the socket
				dug_helper.createSocket();
				// set up socket address
				dug_helper.setupAddress();
				// call connect
				// dug_helper.makeConnection();
				// sending packet query
				dug_helper.sendPacket();
				// recieve the packet answer
				int returnValue = dug_helper.getPacket();
				sleep(1);
			}
	}
	else {
		dug_helper.setQueryType (userQueryResponse);
		// create the Query Header
		dug_helper.createQueryHeader();
		// create the Query Question
		dug_helper.createQueryQuestion();
		// create the socket
		dug_helper.createSocket();
		// set up socket address
		dug_helper.setupAddress();
		// call connect
		//dug_helper.makeConnection();
		// sending packet query
		dug_helper.sendPacket();
		// recieve the packet answer
		int returnValue = dug_helper.getPacket();
	}



	return 0;
}
