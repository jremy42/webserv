#include "Client.hpp"

std::string	Client::_stateStr[4] = {"S_INIT", "S_REQREAD", "S_RESWRITE", "S_OVER"};

Client::Client()
{
	_clientFd = 0;
	_request = NULL;
	_response = NULL;
	_state = S_INIT;
	_config = new Config("1000", "toto");
}

Client::Client(int clientFd, Config* config)
{
	_clientFd = clientFd;
	_request = NULL;
	_response = NULL;\
	_config = config;
	std::cout << "create client with fd :" << _clientFd << std::endl;
	_state = S_INIT;
}

Client::~Client(){};

Client::Client(const Client & src)
{
	if (this != &src)
	{
		*this = src;
	}
};

Client &Client::operator=(const Client &src)
{
	if (this != &src)
	{
		_clientFd = src._clientFd;
		_state = src._state;
		_availableActions = src._availableActions;
		_request = src._request;
		_response = src._response;
		_config = src._config;

	}
	return *this;
};

void	Client::setAvailableActions(int epollFlags)
{
	_availableActions = epollFlags;
}

int Client::executeAction()
{
	int	actionReturnValue;
	int	actionMade = 0;

	// std::cout << "Client State at beginning of executeAction :" <<  getStateStr() << std::endl;
	// printf(" Client_fd:[%d], events [%s][%s][%s][%s][%s]\n", _clientFd,
	// 	(_availableActions & EPOLLIN) ? "EPOLLIN " : "",
	// 	(_availableActions & EPOLLOUT) ? "EPOLLOUT " : "",
	// 	(_availableActions & EPOLLERR) ? "EPOLLERR " : "",
	// 	(_availableActions & EPOLLRDHUP) ? "EPOLLRDHUP " : "",
	// 	(_availableActions & EPOLLHUP) ? "EPOLLHUP " : "");
	if (_availableActions & EPOLLERR)
		return S_CLOSE_FD;
	if ((_availableActions & EPOLLIN) && (_state == S_INIT))
	{
		_request = new Request(_clientFd);
		_state = S_REQREAD;
	}
	if ((_availableActions & EPOLLIN) && _state == S_REQREAD)
	{
		_state = S_REQREAD;
		actionReturnValue = _request->readClientRequest();
		if (actionReturnValue == R_END || actionReturnValue == R_ERROR)
		{
			std::cout << "client getrootDir:[" << _config->getRootDir() << "]\n";
			_response = new Response(_clientFd, _request,_config);
			_state = S_RESWRITE;
			_response->createResponse();
		}
		if (actionReturnValue == R_ZERO_READ)
			_state = S_CLOSE_FD;
		actionMade++;
	}
	else if((_availableActions & EPOLLOUT) && _state == S_RESWRITE)
	{
		if (_response->writeClientResponse() == 0)
			_state = S_OVER;
		actionMade++;
	}
	if(_state == S_OVER || _state == S_CLOSE_FD)
	{
		delete _request;
		delete _response;
		_state = S_INIT;
	}
	std::cout << "Client State at end of executeAction :" <<  getStateStr() << std::endl;
	return (actionMade);
}

int Client::getClientFd(void) const
{
	return _clientFd;
}

int Client::getState(void) const
{
	return (_state);
}

std::string &Client::getStateStr(void) const
{
	return(_stateStr[_state]);
}