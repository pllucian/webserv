#include "Client.hpp"

Client::Client(void) : _fd(-1), _address("") {}

Client::Client(int fd, std::string address) : _fd(fd), _address(address) {}

Client::Client(const Client& src)
{
	*this = src;
}

Client::~Client() {}

Client&	Client::operator=(const Client& rhs)
{
	if (this != &rhs)
	{
		_fd = rhs._fd;
		_address = rhs._address;
		_request = rhs._request;
		_response = rhs._response;
	}
	return *this;
}

int	Client::fd(void)
{
	return _fd;
}

std::string&	Client::address(void)
{
	return _address;
}

Request&	Client::request(void)
{
	return _request;
}

Response&	Client::response(void)
{
	return _response;
}
