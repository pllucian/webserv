#include "Config.hpp"

Config::Config(void) : _host(""), _port(""), _client_max_body_size(0)
{
	_locations["/"] = Location("/");
}

Config::Config(const std::string& config) : _host(""), _port(""), _client_max_body_size(0)
{
	_locations["/"] = Location("/");
	parse_config(config);
}

Config::Config(const Config& src)
{
	*this = src;
}

Config::~Config() {}

Config&	Config::operator=(const Config& rhs)
{
	if (this != &rhs)
	{
		_host = rhs._host;
		_port = rhs._port;
		_server_names = rhs._server_names;
		_error_pages = rhs._error_pages;
		_client_max_body_size = rhs._client_max_body_size;
		_locations = rhs._locations;
	}
	return *this;
}

std::string&	Config::host(void)
{
	return _host;
}

std::string&	Config::port(void)
{
	return _port;
}

std::set<std::string>&	Config::server_names(void)
{
	return _server_names;
}

std::map<int, std::string>&	Config::error_pages(void)
{
	return _error_pages;
}

unsigned long&	Config::client_max_body_size(void)
{
	return _client_max_body_size;
}

std::map<std::string, Location>&	Config::locations(void)
{
	return _locations;
}


void	Config::parse_config(const std::string& config)
{
	std::string	directive;
	size_t		begin;
	size_t		end;

	begin = 0;
	while (begin != std::string::npos)
	{
		if (begin)
			++begin;
		begin = config.find_first_not_of("\t\n\v\f\r ", begin);
		if (begin == std::string::npos)
			break;
		end = config.find_first_of("\t\n\v\f\r ;", begin);
		directive = config.substr(begin, end - begin);
		if (directive == "listen")
			parse_listen(config, begin, end);
		else if (directive == "server_name")
			parse_server_name(config, begin, end);
		else if (directive == "error_page")
			parse_error_page(config, begin, end);
		else if (directive == "client_max_body_size")
			parse_client_max_body_size(config, begin, end);
		else if (directive == "location")
			parse_location(config, begin, end);
		else
			throw std::runtime_error("Error: invalid config file");
	}
}

void	Config::parse_listen(const std::string& config, size_t& begin, size_t& end)
{
	begin = config.find_first_not_of("\t\n\v\f\r ", end);
	end = config.find_first_of("\t\n\v\f\r :;", begin);
	if (config.compare(end, 1, ":"))
		throw std::runtime_error("Error: invalid config file");
	_host = config.substr(begin, end - begin);
	begin = ++end;
	end = config.find_first_of("\t\n\v\f\r ;", begin);
	_port = config.substr(begin, end - begin);
	for (const char* tmp = _port.c_str(); *tmp; ++tmp)
	{
		if (!isdigit(*tmp))
			throw std::runtime_error("Error: invalid config file");
	}
	if (config[end] != ';')
		end = config.find_first_not_of("\t\n\v\f\r ", end);
	if (config[end] != ';')
		throw std::runtime_error("Error: invalid config file");
	begin = end;
}

void	Config::parse_server_name(const std::string& config, size_t& begin, size_t& end)
{
	while (1)
	{
		begin = config.find_first_not_of("\t\n\v\f\r ", end);
		end = config.find_first_of("\t\n\v\f\r ;", begin);
		if (!config.compare(end, 1, ";"))
		{
			if (begin != end)
				_server_names.insert(config.substr(begin, end - begin));
			if (_server_names.empty())
				throw std::runtime_error("Error: invalid config file");
			begin = end;
			break;
		}
		_server_names.insert(config.substr(begin, end - begin));
	}
}

void	Config::parse_error_page(const std::string& config, size_t& begin, size_t& end)
{
	begin = config.find_first_not_of("\t\n\v\f\r ", end);
	end = config.find_first_of("\t\n\v\f\r ;", begin);
	if (!config.compare(end, 1, ";"))
		throw std::runtime_error("Error: invalid config file");

	std::string	error_code = config.substr(begin, end - begin);

	for (const char* tmp = error_code.c_str(); *tmp; ++tmp)
	{
		if (!isdigit(*tmp))
			throw std::runtime_error("Error: invalid config file");
	}
	begin = config.find_first_not_of("\t\n\v\f\r ", end);
	end = config.find_first_of("\t\n\v\f\r ;", begin);
	if (begin == end)
		throw std::runtime_error("Error: invalid config file");

	std::string	error_uri = config.substr(begin, end - begin);

	_error_pages.insert(std::make_pair(atoi(error_code.c_str()), error_uri));
	if (config[end] != ';')
		end = config.find_first_not_of("\t\n\v\f\r ", end);
	if (config[end] != ';')
		throw std::runtime_error("Error: invalid config file");
	begin = end;
}

void	Config::parse_client_max_body_size(const std::string& config, size_t& begin, size_t& end)
{
	begin = config.find_first_not_of("\t\n\v\f\r ", end);
	end = config.find_first_of("\t\n\v\f\r ;", begin);
	if (begin == end)
		throw std::runtime_error("Error: invalid config file");

	std::string	client_max_body_size = config.substr(begin, end - begin);

	for (const char* tmp = client_max_body_size.c_str(); *tmp; ++tmp)
	{
		if (!isdigit(*tmp))
			throw std::runtime_error("Error: invalid config file");
	}
	_client_max_body_size = atoi(client_max_body_size.c_str());
	if (config[end] != ';')
		end = config.find_first_not_of("\t\n\v\f\r ", end);
	if (config[end] != ';')
		throw std::runtime_error("Error: invalid config file");
	begin = end;
}

void	Config::parse_location(const std::string& config, size_t& begin, size_t& end)
{
	begin = config.find_first_not_of("\t\n\v\f\r ", end);
	end = config.find_first_of("\t\n\v\f\r {", begin);
	if (begin == end)
		throw std::runtime_error("Error: invalid config file");
	
	std::string	path = config.substr(begin, end - begin);

	_locations[path].path() = path;
	if (config[end] != '{')
		end = config.find_first_not_of("\t\n\v\f\r ", end);
	if (config[end] != '{')
		throw std::runtime_error("Error: invalid config file");
	begin = ++end;

	std::string	location_directive;

	while (config[begin] != '}')
	{
		if (config[begin] == ';')
			++begin;
		begin = config.find_first_not_of("\t\n\v\f\r ", begin);
		if (config[begin] == '}')
			break;
		end = config.find_first_of("\t\n\v\f\r ;", begin);
		location_directive = config.substr(begin, end - begin);
		if (location_directive == "methods")
			parse_location_methods(config, _locations[path], begin, end);
		else if (location_directive == "return")
			parse_location_return(config, _locations[path], begin, end);
		else if (location_directive == "root")
			parse_location_root(config, _locations[path], begin, end);
		else if (location_directive == "autoindex")
			parse_location_autoindex(config, _locations[path], begin, end);
		else if (location_directive == "index")
			parse_location_index(config, _locations[path], begin, end);
		else if (location_directive == "cgi_pass")
			parse_location_cgi_pass(config, _locations[path], begin, end);
		else if (location_directive == "upload_pass")
			parse_location_upload_pass(config, _locations[path], begin, end);
		else
			throw std::runtime_error("Error: invalid config file");
		begin = end;
	}
}

void	Config::parse_location_methods(const std::string& config, Location& location, size_t& begin, size_t& end)
{
	while (1)
	{
		begin = config.find_first_not_of("\t\n\v\f\r ", end);
		end = config.find_first_of("\t\n\v\f\r ;}", begin);
		if (!config.compare(end, 1, "}"))
			throw std::runtime_error("Error: invalid config file");
		if (!config.compare(end, 1, ";"))
		{
			if (begin != end)
			{
				std::string	method = config.substr(begin, end - begin);

				if (!(method == "GET" || method == "HEAD" || method == "POST" \
					|| method == "PUT" || method == "DELETE"))
					throw std::runtime_error("Error: invalid config file");
				location.methods().insert(method);
			}
			if (location.methods().empty())
				throw std::runtime_error("Error: invalid config file");
			begin = end;
			break;
		}

		std::string	method = config.substr(begin, end - begin);

		if (!(method == "GET" || method == "HEAD" || method == "POST" \
			|| method == "PUT" || method == "DELETE"))
			throw std::runtime_error("Error: invalid config file");
		location.methods().insert(method);
	}
}

void	Config::parse_location_return(const std::string& config, Location& location, size_t& begin, size_t& end)
{
	begin = config.find_first_not_of("\t\n\v\f\r ", end);
	end = config.find_first_of("\t\n\v\f\r ;}", begin);
	if (!config.compare(end, 1, "}"))
		throw std::runtime_error("Error: invalid config file");

	std::string	return_code = config.substr(begin, end - begin);

	for (const char* tmp = return_code.c_str(); *tmp; ++tmp)
	{
		if (!isdigit(*tmp))
			throw std::runtime_error("Error: invalid config file");
	}
	if (!config.compare(end, 1, ";"))
		location.return_() = std::make_pair(atoi(return_code.c_str()), "");
	else
	{
		begin = config.find_first_not_of("\t\n\v\f\r ", end);
		end = config.find_first_of("\t\n\v\f\r ;", begin);
		location.return_() = std::make_pair(atoi(return_code.c_str()), config.substr(begin, end - begin));
	}
	if (config[end] != ';')
		end = config.find_first_not_of("\t\n\v\f\r ", end);
	if (config[end] != ';')
		throw std::runtime_error("Error: invalid config file");
	begin = end;
}

void	Config::parse_location_root(const std::string& config, Location& location, size_t& begin, size_t& end)
{
	begin = config.find_first_not_of("\t\n\v\f\r ", end);
	end = config.find_first_of("\t\n\v\f\r ;}", begin);
	if (!config.compare(end, 1, "}"))
		throw std::runtime_error("Error: invalid config file");
	if (begin == end)
		throw std::runtime_error("Error: invalid config file");
	location.root() = config.substr(begin, end - begin);
	if (config[end] != ';')
		end = config.find_first_not_of("\t\n\v\f\r ", end);
	if (config[end] != ';')
		throw std::runtime_error("Error: invalid config file");
	begin = end;
}

void	Config::parse_location_autoindex(const std::string& config, Location& location, size_t& begin, size_t& end)
{
	begin = config.find_first_not_of("\t\n\v\f\r ", end);
	end = config.find_first_of("\t\n\v\f\r ;}", begin);
	if (!config.compare(end, 1, "}"))
		throw std::runtime_error("Error: invalid config file");
	if (begin == end)
		throw std::runtime_error("Error: invalid config file");
	if (config.substr(begin, end - begin) == "on")
		location.autoindex() = true;
	else if (config.substr(begin, end - begin) == "off")
		location.autoindex() = false;
	else
		throw std::runtime_error("Error: invalid config file");
	if (config[end] != ';')
		end = config.find_first_not_of("\t\n\v\f\r ", end);
	if (config[end] != ';')
		throw std::runtime_error("Error: invalid config file");
	begin = end;
}

void	Config::parse_location_index(const std::string& config, Location& location, size_t& begin, size_t& end)
{
	while (1)
	{
		begin = config.find_first_not_of("\t\n\v\f\r ", end);
		end = config.find_first_of("\t\n\v\f\r ;}", begin);
		if (!config.compare(end, 1, "}"))
			throw std::runtime_error("Error: invalid config file");
		if (!config.compare(end, 1, ";"))
		{
			if (begin != end)
				location.index().push_back(config.substr(begin, end - begin));
			if (location.index().empty())
				throw std::runtime_error("Error: invalid config file");
			begin = end;
			break;
		}
		location.index().push_back(config.substr(begin, end - begin));
	}
}

void	Config::parse_location_cgi_pass(const std::string& config, Location& location, size_t& begin, size_t& end)
{
	begin = config.find_first_not_of("\t\n\v\f\r ", end);
	end = config.find_first_of("\t\n\v\f\r ;}", begin);
	if (!config.compare(end, 1, "}"))
		throw std::runtime_error("Error: invalid config file");
	if (begin == end)
		throw std::runtime_error("Error: invalid config file");
	location.cgi_pass() = config.substr(begin, end - begin);
	if (config[end] != ';')
		end = config.find_first_not_of("\t\n\v\f\r ", end);
	if (config[end] != ';')
		throw std::runtime_error("Error: invalid config file");
	begin = end;
}

void	Config::parse_location_upload_pass(const std::string& config, Location& location, size_t& begin, size_t& end)
{
	begin = config.find_first_not_of("\t\n\v\f\r ", end);
	end = config.find_first_of("\t\n\v\f\r ;}", begin);
	if (!config.compare(end, 1, "}"))
		throw std::runtime_error("Error: invalid config file");
	if (begin == end)
		throw std::runtime_error("Error: invalid config file");
	location.upload_pass() = config.substr(begin, end - begin);
	if (config[end] != ';')
		end = config.find_first_not_of("\t\n\v\f\r ", end);
	if (config[end] != ';')
		throw std::runtime_error("Error: invalid config file");
	begin = end;
}
