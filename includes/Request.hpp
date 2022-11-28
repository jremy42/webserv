#ifndef REQUEST_HPP
#define REQUEST_HPP

#define READ_BUFFER_SIZE 512
# define MAX_REQUESTLINE_SIZE 4096
# define MAX_HEADER_SIZE 4096

# include <string>
# include <vector>
# include <map>
# include <unistd.h>
# include <stdexcept>
# include <cstring>
# include <errno.h>
# include <iostream>
# include <sstream>
# include <iterator>
# include <iomanip>
# include <fstream>
# include <stdlib.h>
# include <string.h>
# include "Config.hpp"

# define VALID_REQUEST_N 3
# define HEADER_FIELD 3
# define REQUEST_LINE_FIELD 3

# ifndef DEBUG_REQUEST
#  define DEBUG_REQUEST 1
# endif

enum {R_REQUESTLINE, R_HEADER, R_SET_CONFIG, R_INIT_BODY_FILE, R_BODY, R_END, R_ERROR, R_ZERO_READ};

class Config;

class Request
{
	public:
		typedef std::string					string;
		typedef std::map<string, string>	m_ss;
		typedef std::vector<char>			v_c;
		typedef v_c::iterator				v_c_it;
		typedef std::vector<Config> 		v_config;


	public:

		Request(void);
		Request(int clientFd, v_config *configList);
		Request(const Request &src);
		Request &operator=(const Request &rhs);
		~Request(void);

		int			readClientRequest(void);
		int			getState(void) const;
		string		&getStateStr(void) const;
		string		getMethod(void) const;
		string		getProtocol(void) const;
		string		getTarget(void) const;
		int			getStatusCode(void) const;
		void 		reset(void);
		string		getHost(void) const;
		string		getTmpBodyFile(void) const;
		void		setClientMaxBodySize(int clientMaxBodySize);
		void		setState(int state);
		int			handleRequest(void);
		const Config		*getMatchingConfig(void) const;
		const Config		*getRequestConfig(void) const;
		const Config		*getConfig(void) const;



	private:

		int				_state;
		int				_clientFd;
		int				_statusCode;
		int				_maxRead;
		m_ss			_requestLine;
		m_ss			_header;
		//v_c				_body;
		v_c				_rawRequest;
		string			_rawRequestString;
		int				_readRet;
		string			_rawRequestLine;
		int				_clientMaxBodySize;
		v_config		*_configList;
		const Config			*_config;
		static string	_requestLineField[3];
		static string	_headerField[3];
		static string	_validRequest[3];
		static string	_stateStr[8];

		void	_handleRequestLine(void);
		void	_handleHeader(void);
		void	_handleBody(void);
		int		parseRequestLine(string rawRequestLine);
		int		parseHeader(string rawRequestLine);
		int		checkRequestLine(void);
		int		checkHeader(void);
		void	_setConfig(void);

		//handle body
		void _initBodyFile(void);
		string _nameBodyFile;
		//int	 _bodyFile;
		int  _bodyFileSize;
		std::ofstream _fs;
};

std::string	&strtrim(std::string &str, const std::string &charset);

#endif
