#include "WebServ.hpp"

WebServ::WebServ(void) : Http(), _epfd(-1) {}

WebServ::WebServ(const std::string& path) : Http(), _epfd(-1)
{
	init_servers(path);
}

WebServ::WebServ(const WebServ& src) : Http(src)
{
	*this = src;
}

WebServ::~WebServ() {}

WebServ&	WebServ::operator=(const WebServ& rhs)
{
	if (this != &rhs)
	{
		_epfd = rhs._epfd;
		_servers = rhs._servers;
		_clients = rhs._clients;
	}
	return *this;
}

void	WebServ::start(void)
{
	init_epoll();
	main_loop();
}

void	WebServ::init_servers(const std::string& path)
{
	std::ifstream	ifs(path.c_str());
	std::string		config;
	std::string		line;
	size_t			begin;
	size_t			end;
	int				block;

	if (!ifs.is_open())
		throw std::ios_base::failure("Error: can not open config file");
	while (getline(ifs, line))
		config.append(line);
	ifs.close();
	begin = 0;
	while (begin != std::string::npos)
	{
		if (begin)
			++begin;
		begin = config.find_first_not_of("\t\n\v\f\r ", begin);
		if (begin == std::string::npos)
			break;
		if (config.compare(begin, 6, "server"))
			throw std::runtime_error("Error: invalid config file");
		begin = config.find_first_not_of("\t\n\v\f\r ", begin + 6);
		if (begin == std::string::npos || config.compare(begin, 1, "{"))
			throw std::runtime_error("Error: invalid config file");
		end = begin++;
		block = 0;
		while (1)
		{
			end = config.find_first_of("{}", ++end);
			if (end == std::string::npos)
				throw std::runtime_error("Error: invalid config file");
			if (!config.compare(end, 1, "{"))
				++block;
			else if (!config.compare(end, 1, "}") && block)
				--block;
			else if (!config.compare(end, 1, "}") && !block)
				break;
		}
		try
		{
			Server	serv = Server(config.substr(begin, end - begin));

			_servers[serv.servfd()] = serv;
		}
		catch(const std::exception& e)
		{
			if (std::string(e.what()) != std::string("Error: bind"))
				throw std::runtime_error(e.what());
		}
		begin = end;
	}
}

void	WebServ::init_epoll(void)
{
	epoll_event	ev;

	_epfd = epoll_create(MAX_EVENTS);
	if (_epfd < 0)
		throw std::runtime_error("Error: epoll_create");
	memset(&ev, 0, sizeof(ev));

	std::map<int, Server>::iterator	it;
	ev.events = EPOLLIN;

	for (it = _servers.begin(); it != _servers.end(); ++it)
	{
		ev.data.fd = (*it).second.servfd();
		if (epoll_ctl(_epfd, EPOLL_CTL_ADD, ev.data.fd, &ev) < 0)
			throw std::runtime_error("Error: epoll_ctl");
	}
}

void	WebServ::main_loop(void)
{
	int			nfds;
	epoll_event	events[MAX_EVENTS];

	if (_servers.empty())
		throw std::runtime_error("No servers to start");
	while (1)
	{
		nfds = epoll_wait(_epfd, events, MAX_EVENTS, -1);
		if (nfds < 0)
			throw std::runtime_error("Error: epoll_wait");
		for (int i = 0; i < nfds ; ++i)
		{
			if (events[i].events & EPOLLERR \
				|| events[i].events & EPOLLHUP)
			{
				if (_clients.count(events[i].data.fd))
				{
					std::cout << "Client #" << events[i].data.fd << " gone away\n";
					close(events[i].data.fd);
					_clients[events[i].data.fd]->del_client(events[i].data.fd);
					_clients.erase(events[i].data.fd);
				}
			}
			else
			{
				if (_servers.count(events[i].data.fd))
					accept_client(events[i].data.fd);
				else
					handle_client(events[i].data.fd, events[i].events);
			}
		}
	}
}

void	WebServ::accept_client(int servfd)
{
	int			fd;
	sockaddr	addr;
	socklen_t	addrlen;
	epoll_event	ev;
	char		buf[INET6_ADDRSTRLEN];
	std::string	address;
	int			port;

	memset(&addr, 0, sizeof(addr));
	addrlen = sizeof(addr);
	fd = accept(servfd, &addr, &addrlen);
	if (fd < 0)
		throw std::runtime_error("Error: accept");
	memset(&ev, 0, sizeof(ev));
	ev.events = EPOLLIN | EPOLLOUT;
	ev.data.fd = fd;
	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev) < 0)
		throw std::runtime_error("Error: epoll_ctl");
	memset(buf, 0, INET6_ADDRSTRLEN);
	if (addr.sa_family == AF_INET)
	{
		address = inet_ntoa(((sockaddr_in*)&addr)->sin_addr);
		port = ntohs(((sockaddr_in*)&addr)->sin_port);
	}
	else
	{
		address = inet_ntop(AF_INET6, &(((sockaddr_in6*)&addr)->sin6_addr), buf, INET6_ADDRSTRLEN);
		port = ntohs(((sockaddr_in6*)&addr)->sin6_port);
	}

	Server&	server = _servers[servfd];
	Client	client = Client(fd, address);

	server.add_client(client);
	_clients[fd] = &server;
	std::cout << "New client #" << fd << " from " << address << ":" << port << " on server " \
		<< server.config().host() << ":" << server.config().port() << '\n';
}

void	WebServ::handle_client(int fd, unsigned int events)
{
	ssize_t	ret;
	char	buf[BUFFER_SIZE];
	Client&	client = _clients[fd]->get_client(fd);

	if (events & EPOLLIN)
	{
		ret = recv(fd, buf, BUFFER_SIZE, MSG_DONTWAIT | MSG_NOSIGNAL);
		if (ret <= 0)
		{
			std::cout << "Client #" << client.fd() << " gone away\n";
			close(fd);
			_clients[fd]->del_client(fd);
			_clients.erase(fd);
		}
		else
		{
			client.request().buffer().append(buf, ret);
			http_routine(*_clients[fd], client);
		}
	}
	else if (events & EPOLLOUT)
	{
		ret = client.response().buffer().size();
		if (ret)
		{
			if (ret > BUFFER_SIZE)
				ret = BUFFER_SIZE;
			ret = send(fd, client.response().buffer().c_str(), ret, MSG_DONTWAIT | MSG_NOSIGNAL);
			if (ret <= 0)
			{
				std::cout << "Client #" << client.fd() << " gone away\n";
				close(fd);
				_clients[fd]->del_client(fd);
				_clients.erase(fd);
			}
			else
			{
				client.response().buffer().erase(0, ret);
			}
		}
	}
}
