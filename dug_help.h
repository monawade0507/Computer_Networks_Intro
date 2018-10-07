#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <vector>

#define ENDL " (" << __FILE__ << ":" << __LINE__ << ")"

class DugHelp {
	private:
		// Use htons() and ntohs() to convert multi-byte integers to the correct ordering
		// If there is any doubt, use bitmasks instead
		struct DNS_Header {
			uint16_t id;
			struct {
				uint8_t rd: 1;		// recursion desired
				uint8_t tc: 1;		// message was truncated
				uint8_t aa: 1;		// authoritative answer
				uint8_t opcode: 4;	// query type
				// 0 a standard query (QUERY); 1 an inverse query (IQUERY); 2 a server status request (STATUS); 
				// 3 - 15 reserved for futre use
				uint8_t qr: 1;		// query(0) or response (1)

				uint8_t rcode: 4;	// response code
				// 0: No error condition; 1:Format error; 2:Server failure; 3: Name Error (authoritative server, name doesn't exist)
				// 4: Not implemented; 5: Refused
				uint8_t z: 3;		// reserved for future use. Must be zero
				uint8_t ra: 1;		// recursion
				} flags;
			uint16_t gdcount;
			uint16_t ancount;
			uint16_t nscount;
			uint16_t arcount;
		};
		struct DNS_Question {
			unsigned char *qname;
			uint16_t qtype;
			uint16_t qclass;
		};



		std::string hostname;
		std::string IPaddress;
		std::string queryType = "";
		std::string qname_labelFormat;
		DNS_Header packetHeader;
		DNS_Question packetQuestion;

	public:
		DugHelp();
		void setHostName(std::string name);
		void setIPaddress(std::string addr);
		std::string getHostName();
		std::string getIPaddress();
		void setQueryType(std::string type);
		void createQueryHeader();
		void stringToHex();					// converting the hostname to qname (length/data pair)
		void createQueryQuestion();				
		void createQueryMessage();				// Message = Header + Question
		DNS_Header returnDNSHeader() { return packetHeader; }
		DNS_Question returnDNSQuestion() { return packetQuestion; }

};


