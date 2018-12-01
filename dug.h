// ********************************************************
// * A common set of system include files needed for socket() programming
// ********************************************************
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
#include <getopt.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include "dug_help.h"

#define ENDL  " (" << __FILE__ << ":" << __LINE__ << ")"
