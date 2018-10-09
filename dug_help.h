#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <vector>
#include <sstream>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <cstring>
#include "log.h"

#define ENDL " (" << __FILE__ << ":" << __LINE__ << ")"

class DugHelp {
	private:
		/**************************************************************************
		struct DNS Header
		*/
		struct DNS_Header {
			unsigned short id;
			struct {
				unsigned char rd: 1;		// recursion desired
				unsigned char tc: 1;		// message was truncated
				unsigned char aa: 1;		// authoritative answer
				unsigned char opcode: 4;	// query type
				// 0 a standard query (QUERY); 1 an inverse query (IQUERY);
				// 2 a server status request (STATUS);
				// 3 - 15 reserved for futre use
				unsigned char qr: 1;		// query(0) or response (1)

				unsigned char rcode: 4;	// response code
				// 0: No error condition; 1:Format error; 2:Server failure;
				// 3: Name Error (authoritative server, name doesn't exist)
				// 4: Not implemented; 5: Refused
				unsigned char z: 1;		// reserved for future use. Must be zero
				unsigned char ra: 1;		// recursion
				} flags;
			unsigned short gdcount;
			unsigned short ancount;
			unsigned short nscount;
			unsigned short arcount;
		};
		/**************************************************************************
		struct DNS Question Data
		*/
		struct DNS_Question_Data {
			unsigned short qtype;
			unsigned short qclass;
		};
		/**************************************************************************
		struct DNS answer data
		*/
		struct DNS_Answer_Data {
			unsigned short type;
			unsigned short _class;
			unsigned int ttl;
			unsigned int len;
		};
		/**************************************************************************
		struct DNS answer
		*/
		struct DNS_Answer {
			unsigned char name[255];
			struct {
				unsigned short type;
				unsigned short _class;
				unsigned int ttl;
				unsigned int len;
			} types;
			unsigned char data[2000];
		};
		/**************************************************************************
		DNS required variables
		*/
		std::string hostname;
		std::string IPaddress;
		std::string queryType = "";

		struct DNS_Header *dnsHeader = NULL;
		struct DNS_Question *dnsQuestion = NULL;
		struct DNS_Answer *dnsAnswer = NULL;
		/***************************************************************************
		UDP Socket required struct
		struct sockaddr_in
		*/
		struct sockaddr_in
		{
			short     	family;				// address family
			u_short       	port;				// port number
			struct in_addr  sin_addr;			// internet address
			char 		sin_zero[8];
		};
		/**************************************************************************
		UDP Socket required variables
		*/
		int sock = -1;						// file descriptor for the server
		int lst  = -1;						// file descriptor for the listener
		struct sockaddr_in servAddr;				// struct needed for the server addr
		struct sockaddr_in clientAddr;			// struct needed for the listener addr
		char buffer[256];
		int portNum = -1;
		int n = 0;
		Log* logger = new Log();
		unsigned char buf[2000], *reader;
		unsigned char qname[255];
		int qnameSize = 0;

	public:
		/*************************************************************************
		UDP Socket Required functions
		*/
		void createSocket();
		void setupAddress();
		void sendPacket();
		int getPacket();
		/*************************************************************************
		DNS Required functions
		*/
		DugHelp();
		void setHostName(std::string name);
		void setIPaddress(std::string addr);
		std::string getHostName();
		std::string getIPaddress();
		void setQueryType(std::string type);
		void createQueryHeader();
		void stringToDec();	  // converting the hostname to qname (length/data pair)
		void createQueryQuestion();
};
