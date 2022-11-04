#include "Config.hpp"

std::map<std::string, int>	Config::_configField = _initConfigField();

std::map<std::string, int>	Config::_initConfigField()
{
	std::map<string, int> configField;

	configField.insert(std::pair<std::string, int>("listen", 1));
	configField.insert(std::pair<std::string, int>("root", 1));
	configField.insert(std::pair<std::string, int>( "server_name", 0));
	return (configField);
}

Config::Config(void)
{

};

Config::Config(std::string rawServerConfig)
{
	while(rawServerConfig.at(0) != '{')
		rawServerConfig.erase(0,1);
	rawServerConfig.erase(0,1);
	rawServerConfig.erase(rawServerConfig.end() - 1);
	if (DEBUG_CONFIG)
		std::cout << "rawServerConfig : >" << rawServerConfig << "<" << std::endl;
	_serverInfoMap = _createServerInfoMap(rawServerConfig);

	if (DEBUG_CONFIG)
		std::cout << " map size serverInfoMap: [" << _serverInfoMap.size() <<"]" << std::endl;
	 if (DEBUG_CONFIG)
		std::cout << "Found a viable 'server {}' conf : >" << rawServerConfig << "<" << std::endl;
	if (_serverInfoMap.empty())
		throw(std::runtime_error("webserv: empty conf, server block ignored"));
	std::vector<string> test = _serverInfoMap["listen"];
	
	if (DEBUG_CONFIG)
		std::cout << "test" << test.front() << std::endl;
	//if (DEBUG_CONFIG)
	//	std::cout << "Calling atoi on :" << _serverInfoMap["listen"][0] << std::endl;
	_listenPort = atoi(_serverInfoMap["listen"][0].c_str());
	if (DEBUG_CONFIG)
		std::cout << "Atoi value : " << _listenPort << std::endl;
		// A integrer dans un fx de verif du port !
	if (_listenPort < 1024)
		throw(std::runtime_error("webserv: Wrong Port : " + _itoa(_listenPort) + ". Value must be above 1024"));
	if (_listenPort > 65536)
		throw(std::runtime_error("webserv: Wrong Port : " + _itoa(_listenPort) + ". Value must be below 65536"));
	_rootDirectory = _serverInfoMap["root"][0];
	// A integrer dans un fx de verif du port !
	//Fonction de check du path a faire (ou pas ? on aura une 404 ?)
	
	//rawServerConf = getNextServerBlock(*it);
};


Config::Config(const Config &src)
{
	*this = src;
}

Config::~Config(void)
{

}

Config	&Config::operator=(const Config &rhs)
{
	this->_listenPort = rhs._listenPort;
	this->_rootDirectory = rhs._rootDirectory;
	this->_serverInfoMap = rhs._serverInfoMap;
	this->_location = rhs._location;

	return (*this);
}

const std::string		Config::getListenPortStr(void) const
{
	return (_itoa(_listenPort));
}

const std::vector<std::string> Config::getServerName(void) const
{
	std::cout << " return of getServerName:"<< _serverInfoMap.find("server_name")->second << std::endl;
	return (_serverInfoMap.find("server_name")->second) ;
}

int		Config::getListenPort(void) const
{
	return (_listenPort);
}

const char* Config::getRootDir(void) const
{
	return (_rootDirectory.c_str());
}

char	Config::getNextBlockDelim(std::string str, int pos) const
{
	char			nextBlockDelim = '0';
	std::size_t		nextPosDelim;								
	string			tmp(str.begin() + pos, str.end());

	if ((nextPosDelim = tmp.find_first_of("{;") )!= std::string::npos)
	{
		nextBlockDelim = tmp[nextPosDelim];
		nextBlockDelim = (nextBlockDelim == ';') ? ';' : '}';
	}
	return (nextBlockDelim);
}



std::map<std::string, std::vector<std::string> > Config::_createServerInfoMap(std::string &rawServerConf)
{
	m_s_vs											serverInfoMap;
	std::istringstream								istr(rawServerConf);
	std::string										nextLine;
	std::size_t										nextBlankToReplace;
	std::size_t										replaceOffset;
	//int												missingMandatoryField = _configField.size(); // A recup pour les tableaux des autres classes
	char											nextBlockDelim = '0';
	std::pair<string, std::vector<string> >			configLine;
	//init serverInfoMap
	for (std::map<std::string, int>::iterator it = _configField.begin(); it != _configField.end(); it++)
		serverInfoMap.insert(std::pair<std::string, std::vector<std::string> >((*it).first, std::vector<string>()));
	//init serverInfoMap
	nextBlockDelim = getNextBlockDelim(istr.str(), istr.tellg());
	while ( nextBlockDelim != '0' && getline(istr, nextLine, nextBlockDelim))
	{
		//std::cout << "stream position: [" << istr.tellg() << "]\n";
		if (nextBlockDelim == '}')
		{
			try
			{
				string key(getNextLocationBlock(nextLine));
				Location newLocation(nextLine);
				if (DEBUG_CONFIG)
					std::cout << "\e[33m key :[" << key << "]\n";
				_location.insert(std::pair<string, Location>(key, newLocation));
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
			
			if (DEBUG_CONFIG)
				std::cout << "nextLine inside {} : [" << nextLine << "]" <<std::endl;
		}
		else
		{
			replaceOffset = 0;
			strtrim(nextLine, "\f\t\r\v\n ;");
			// On remplace tout les blank par un seul blanck
			while (replaceOffset < nextLine.length()
					&& (nextBlankToReplace = nextLine.find_first_of("\f\t\r\v\n ", replaceOffset)) != std::string::npos)
			{
				nextLine.at(nextBlankToReplace) = ' ';
				replaceOffset = nextBlankToReplace + 1;
				while (replaceOffset < nextLine.length()
						&& std::string("\f\t\r\v\n ").find_first_of(nextLine.at(replaceOffset)) != std::string::npos)
					nextLine.erase(replaceOffset, 1);
			}
			// On remplace tout les blank par un seul blanck
			configLine = parseConfigBlock(nextLine);
			if (DEBUG_CONFIG)
				std::cout << "\e[31m configLine :" << configLine << "\e[0m\n"; 
			serverInfoMap[configLine.first] = configLine.second;
		}
		if (istr.tellg() < 0)
			break;
		nextBlockDelim = getNextBlockDelim(istr.str(), istr.tellg());
		std::cout << "\e[32m nexBlockDelim [" << nextBlockDelim << "] \e[0m" << std::endl;
	}
	/*
	   if (missingMandatoryField != 0)
	   {
	   serverInfoMap.clear();
	   std::cerr << "Too " << (missingMandatoryField > 0 ? "few" : "many") << " mandatory Server Info key-values" << std::endl;
	   }
	   else if (DEBUG_CONFIG)
	   std::cout << "Valid ServerInfoArray !" << std::endl;
	 */
	std::cout << "inserted a new Config maps " << serverInfoMap << "" <<std::endl;
	return (serverInfoMap);
}

std::pair<std::string, std::vector<std::string > >	Config::parseConfigBlock(std::string &nextLine)
{
	std::pair<string, std::vector<string> >		ret;
	std::istringstream							iss(nextLine);
	string										key;
	string										value;

	getline(iss, key, ' ');
	if (_configField.find(key) != _configField.end())
	{
		if (DEBUG_CONFIG)
			std::cout << "Found a mandatory Field : [" << key << "]" << std::endl;
		ret.first = key;
		while (getline(iss, value, ' '))
		{
			if (DEBUG_CONFIG)
				std::cout << "\e[33m add value:[" << value << "\e[0m\n";
			ret.second.push_back(value);
		}
	}
	else
	{
		if (DEBUG_CONFIG)
			std::cout << "No such Config Key " << key << std::endl;
	}
	return (ret);
}


std::string	Config::getNextLocationBlock(std::string &rawLocation)
{
	std::string							rawLocationConf;
	std::string							whiteSpaces("\f\t\n\r\v ");
	std::size_t							startLocationWord;
	std::size_t							openBracket;
	std::string							key;
	startLocationWord = rawLocation.find("location");
	if (startLocationWord == std::string::npos)
		throw(std::runtime_error("webserv: wrong location block"));
	openBracket = rawLocation.find("{", startLocationWord);
	if (openBracket == std::string::npos)
		throw(std::runtime_error("webserv: wrong location block"));
	key = rawLocation.substr(startLocationWord + 8, openBracket - (startLocationWord + 8));
	key = strtrim(key, whiteSpaces);
	if (key.size() == 0 || key.find_first_of(whiteSpaces) != std::string::npos)
		throw(std::runtime_error("webserv: wrong location block"));
	rawLocationConf = std::string(rawLocation.begin() + openBracket + 1,rawLocation.end());
	if (DEBUG_CONFIG)
		std::cout << "\e[31m rawLocationConf: [" << rawLocationConf << "]\e[0m\n" ;
	rawLocation = rawLocationConf;
	return(key);
}

std::ostream	&operator<<(std::ostream &o, const std::vector<std::string> &vec)
{
	for (unsigned long i = 0; i < vec.size(); i++)
		std::cout << "[" << i << "]->[" << vec[i] << "]";
	return (o);
}

std::ostream	&operator<<(std::ostream &o, const std::pair<std::string, std::vector<std::string> > &pair)
{
	std::cout << "[" << pair.first << "]";
	std::cout << pair.second;
	return (o);
}

std::ostream	&operator<<(std::ostream &o, const std::map<std::string, std::vector<std::string> > &map)
{
	std::map<std::string, std::vector<std::string> >::const_iterator	it = map.begin();
	for (; it != map.end(); it++)
		std::cout << *it << std::endl;
	return (o);
}
