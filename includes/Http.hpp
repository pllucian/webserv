#ifndef HTTP_HPP
# define HTTP_HPP

# include "Server.hpp"
# include <sys/stat.h>
# include <sys/types.h>
# include <unistd.h>
# include <dirent.h>
# include <fcntl.h>
# include <sys/wait.h>

class Http
{
	public:
		Http(void);
		Http(const Http& src);
		~Http();

		Http&	operator=(const Http& rhs);

		void	http_routine(Server& server, Client& client);

	private:
		typedef	void (Http::*Method)(Server& server, Client& client, Location& location);
		
		std::map<std::string, Method>	_methods;

		Location&	find_location(std::string& location, Config& config);
		void		method_get(Server& server, Client& client, Location& location);
		void		method_head(Server& server, Client& client, Location& location);
		void		method_put(Server& server, Client& client, Location& location);
		void		method_post(Server& server, Client& client, Location& location);
		void		method_delete(Server& server, Client& client, Location& location);
		std::string	error_path(Server& server);
		std::string	index_path(Location& location, std::string path);
		std::string	autoindex_page(Client& client, Location& location);
		std::string	cgi_page(Client& client, Location& location, Location& cgi);
		std::string	read_file(const char* path);
		std::string	get_path(Client& client, Location& location);
		std::string	get_upload_path(Client& client, Location& location);
		std::string	cookie_to_query_string(std::string& cookie);
};

#endif
