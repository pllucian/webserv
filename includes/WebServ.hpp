#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "Http.hpp"
# include "Server.hpp"
# include <unistd.h>
# include <sys/epoll.h>
# include <arpa/inet.h>

# define MAX_EVENTS		10000
# define BUFFER_SIZE	32768

class WebServ : public Http
{
	public:
		WebServ(void);
		WebServ(const std::string& path);
		WebServ(const WebServ& src);
		~WebServ();

		WebServ&	operator=(const WebServ& rhs);

		void	start(void);

	private:
		int						_epfd;
		std::map<int, Server>	_servers;
		std::map<int, Server*>	_clients;

		void	init_servers(const std::string& path);
		void	init_epoll(void);
		void	main_loop(void);
		void	accept_client(int servfd);
		void	handle_client(int fd, unsigned int events);
};

#endif
