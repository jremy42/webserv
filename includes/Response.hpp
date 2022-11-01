#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#define WRITE_BUFFER_SIZE	1024
#define BUFF_MAX			1024*1024*8

# include "Request.hpp"
# include <string>
# include <vector>
# include <map>
# include <sstream>
# include <sys/types.h>
# include <sys/socket.h>
# include <fstream>
# include <algorithm>
# include "Config.hpp"


class Response
{
	typedef std::string					string;
	typedef std::map<string, string>	m_ss;
	typedef std::vector<char>			v_c;
	typedef std::map<int, string>		m_is;

	public:
		Response(void);
		Response(int clientFd, Request *request, Config *config);
		Response(const Response &src);
		Response &operator=(const Response &rhs);
		~Response(void);
		void setRequest(const Request *request);
		int	createResponse(void);
		int	writeClientResponse(void);
		void reset(void);

	private:

		int								_responseReady; // CGI donc temps de process possible ?
		int								_clientFd;
		int								_statusCode;
		string							_lineStatus;
		string							_header;
		v_c								_body;
		v_c								_fullResponse;
		string							_bodyToSend; // tmp
		const Request *					_request;
		Config *						_config;
		static std::map<int, string>	_errorMessage;
		static string					_errorBodyTemplate;
		static m_is 					_initErrorMessage(void);
		void							_createErrorMessageBody(void);
		void							_createBody(void);
		void							_createHeader(void);
		void							_createFullResponse(void);


};
 
std::string _itoa(int statusCode);

#endif
