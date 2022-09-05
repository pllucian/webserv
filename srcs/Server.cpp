#include "Server.hpp"

Server::Server(void) : _config(Config()), _host(0), _port(0), _servfd(-1) {}

Server::Server(const std::string& config) : _config(Config(config)), _host(0), _port(0), _servfd(-1)
{
	addrinfo	hints;
	addrinfo*	servinfo;
	addrinfo*	p;
	int			optval = 1;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo(_config.host().c_str(), _config.port().c_str(), &hints, &servinfo) != 0)
		throw std::runtime_error("Error: getaddrinfo");
	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		_servfd = socket(p->ai_family, p->ai_socktype | SOCK_NONBLOCK, p->ai_protocol);
		if (_servfd < 0)
			continue ;
		if (setsockopt(_servfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
		{
			freeaddrinfo(servinfo);
			throw std::runtime_error("Error: setsockopt");
		}
		if (bind(_servfd, p->ai_addr, p->ai_addrlen) < 0)
		{
			freeaddrinfo(servinfo);
			throw std::runtime_error("Error: bind");
		}
		break ;
	}
	freeaddrinfo(servinfo);
	if (!p)
		throw std::runtime_error("Error: socket");
	if (listen(_servfd, SOMAXCONN) < 0)
		throw std::runtime_error("Error: listen");
	std::cout << "Server #" << _servfd << " " << _config.host().c_str() << ":" \
		<< _config.port().c_str() << " started\n";
}

Server::Server(const Server& src)
{
	*this = src;
}

Server::~Server() {}

Server&	Server::operator=(const Server& rhs)
{
	if (this != &rhs)
	{
		_config = rhs._config;
		_host = rhs._host;
		_port = rhs._port;
		_servfd = rhs._servfd;
	}
	return *this;
}

Config&		Server::config(void)
{
	return _config;
}

int	Server::servfd(void)
{
	return _servfd;
}

Client&	Server::get_client(int fd)
{
	return _clients[fd];
}

void	Server::add_client(Client& client)
{
	_clients[client.fd()] = client;
}

void	Server::del_client(int fd)
{
	_clients.erase(fd);
}
