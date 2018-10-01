#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <fcntl.h>

class DugHelp {
	private:
		std::string hostname;
		std::string IPaddress;
	public:
		DugHelp();
		void setHostName(std::string name);
		void setIPaddress(std::string addr);
		std::string getHostName();
		std::string getIPaddress();
};
