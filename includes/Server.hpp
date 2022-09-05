#ifndef SERVER_HPP
# define SERVER_HPP

# include "Config.hpp"
# include "Client.hpp"
# include <cstring>
# include <netdb.h>

class Server
{
	public:
		Server(void);
		Server(const std::string& config);
		Server(const Server& src);
		~Server();

		Server&	operator=(const Server& rhs);

		Config&		config(void);
		int			servfd(void);
		Client&		get_client(int fd);
		void		add_client(Client& client);
		void		del_client(int fd);

	private:
		Config					_config;
		in_addr_t				_host;
		unsigned short int		_port;
		int						_servfd;
		std::map<int, Client>	_clients;
};

#endif
