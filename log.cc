#include "log.h"


void Log::setLogger (bool set) 
{
	if (set) 
	{
		useLogger = set;		// true
	}
	else 
	{
		useLogger = set;		// false
	}
};


void Log::printLog (std::string comment)
{
	if (useLogger) 
	{
		std::cout << comment << std::endl;
	}
};


