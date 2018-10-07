// Simple Logger
// used for simple writing to standard cout when verbose is set
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <fcntl.h>
  
#define ENDL  " (" << __FILE__ << ":" << __LINE__ << ")"

class Log
{
	public:
		void setLogger (bool set);
		void printLog (std::string comment);
		bool getLogger () 
		{
			std::cout << "Logger is set to: " << useLogger << std::endl;
			return true;
		};
		Log() 
		{ 
			useLogger = false; 
		};
	private:
		bool useLogger;
};
