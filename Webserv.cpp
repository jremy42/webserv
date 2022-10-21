#include "Webserv.hpp"
#include "iostream"

Webserv::Webserv(char **configArray)
{
	std::string		nextLine;
	std::ifstream	fs;
	int				viableConfig = 0;

	++configArray;
	while (*configArray)
	{
		fs.open(*configArray, std::ifstream::in);
		if (fs.good())
			std::cout << "Successfully opened config file : '" << *configArray << "'" << std::endl;
		else
		{
			std::cerr << "Failure opening config file : '" << *configArray << "' : " << strerror(errno) << std::endl;
			fs.close();
			break ;
		}
		_rawConfig.push_back("");
		while (std::getline(fs, nextLine))
		{
			if (DEBUG)
			{
				if (nextLine.length() == 0)
					std::cout << "[Empty Line]" << std::endl;
				else
					std::cout << nextLine << std::endl;
			}
			if (nextLine != "")
			{
				_rawConfig.back() += nextLine;
				_rawConfig.back() += "\n";
			}
		}
		fs.close();
		++configArray;
	}
	std::cout << "Config list :" << std::endl;
	for (it = _rawConfig.begin(); it != _rawConfig.end(); it++)
	{
		std::cout << "-----Start Config-----" << std::endl << *it << "------End Config------" << std::endl;
		viableConfig |= (*it != "");
	}
	if (viableConfig == 0)
		throw NotEnoughValidConfigFilesException();
	return ;
}

Webserv::Webserv(const Webserv &src)
{
	*this = src;
}

Webserv::~Webserv(void)
{

}


Webserv &Webserv::operator=(const Webserv &rhs)
{
	this->_serverList = rhs._serverList;
	this->_rawConfig = rhs._rawConfig;
	return (*this);
}

int		Webserv::parseRawConfig(void)
{
	return (1);
}

int		Webserv::createServerListFromRawConfig(void)
{
	return (1);
}

int		Webserv::execServerLoop(void)
{
	return (1);
}

const char	*Webserv::NotEnoughValidConfigFilesException::what(void) const throw ()
{
	return ("Need at least one valid configuration file to launch Webserv");
}
