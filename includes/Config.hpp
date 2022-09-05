#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "Location.hpp"
# include <iostream>
# include <map>
# include <cstdlib>

class Config
{

	public:
		Config(void);
		Config(const std::string& config);
		Config(const Config& src);
		~Config();

		Config&	operator=(const Config& rhs);

		std::string&						host(void);
		std::string&						port(void);
		std::set<std::string>&				server_names(void);
		std::map<int, std::string>&			error_pages(void);
		unsigned long&						client_max_body_size(void);
		std::map<std::string, Location>&	locations(void);

	private:
		std::string						_host;
		std::string						_port;
		std::set<std::string>			_server_names;
		std::map<int, std::string>		_error_pages;
		unsigned long					_client_max_body_size;
		std::map<std::string, Location>	_locations;

		void	parse_config(const std::string& config);
		void	parse_listen(const std::string& config, size_t& begin, size_t& end);
		void	parse_server_name(const std::string& config, size_t& begin, size_t& end);
		void	parse_error_page(const std::string& config, size_t& begin, size_t& end);
		void	parse_client_max_body_size(const std::string& config, size_t& begin, size_t& end);
		void	parse_location(const std::string& config, size_t& begin, size_t& end);
		void	parse_location_methods(const std::string& config, Location& location, \
					size_t& begin, size_t& end);
		void	parse_location_return(const std::string& config, Location& location, \
					 size_t& begin, size_t& end);
		void	parse_location_root(const std::string& config, Location& location, \
					 size_t& begin, size_t& end);
		void	parse_location_autoindex(const std::string& config, Location& location, \
					 size_t& begin, size_t& end);
		void	parse_location_index(const std::string& config, Location& location, \
					 size_t& begin, size_t& end);
		void	parse_location_upload_pass(const std::string& config, Location& location, \
					 size_t& begin, size_t& end);
		void	parse_location_cgi_pass(const std::string& config, Location& location, \
					 size_t& begin, size_t& end);
};

#endif
