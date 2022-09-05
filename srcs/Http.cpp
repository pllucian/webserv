#include "Http.hpp"

Http::Http(void)
{
	_methods["GET"] = &Http::method_get;
	_methods["HEAD"] = &Http::method_head;
	_methods["PUT"] = &Http::method_put;
	_methods["POST"] = &Http::method_post;
	_methods["DELETE"] = &Http::method_delete;
}

Http::Http(const Http& src)
{
	*this = src;
}

Http::~Http() {}

Http&	Http::operator=(const Http& rhs)
{
	if (this != &rhs)
		_methods = rhs._methods;
	return *this;
}

void	Http::http_routine(Server& server, Client& client)
{
	client.request().parse_buffer();
	if (client.request().ready())
	{
		if (!client.request().connection().empty())
			client.response().connection() = client.request().connection();
		if (client.request().error())
		{
			client.response().status(400);
			client.response().body() = read_file("error_pages/400.html");
		}
		else if (client.request().protocol() != "HTTP/1.1")
		{
			client.response().status(505);
			client.response().body() = read_file("error_pages/505.html");
		}
		else
		{
			std::map<std::string, Method>::iterator	it = _methods.find(client.request().method());

			if (it != _methods.end())
			{
				Location&	location = find_location(client.request().location(), server.config());

				if (!location.methods().empty() \
					&& !location.methods().count(client.request().method()))
				{
					client.response().status(405);
					client.response().body() = read_file("error_pages/405.html");
				}
				else if (location.return_().first)
				{
					if (location.return_().first == 404)
					{
						client.response().status(404);
						client.response().body() = read_file(error_path(server).c_str());
					}
					else if (location.return_().first == 301)
					{
						client.response().status(301);
						client.response().location() = location.return_().second;
					}
				}
				else
					(this->*(it->second))(server, client, location);
			}
			else
			{
				client.response().status(400);
				client.response().body() = read_file("error_pages/400.html");
			}
		}
		client.response().add_buffer();
		client.response().reset();
		client.request().reset();
	}
}

Location&	Http::find_location(std::string& location, Config& config)
{
	std::map<std::string, Location>::iterator	it;

	for (std::string tmp = location; tmp.length(); tmp.erase(tmp.find_last_of("/")))
	{
		it = config.locations().find(tmp);
		if (it != config.locations().end())
			break;
	}
	if (it == config.locations().end())
		it = config.locations().find("/");
	return it->second;
}

void	Http::method_get(Server& server, Client& client, Location& location)
{
	std::string	path = get_path(client, location);
	struct stat	stat_buf;

	client.response().status(200);
	if (stat(path.c_str(), &stat_buf) == -1 || S_ISCHR(stat_buf.st_mode) || S_ISBLK(stat_buf.st_mode) \
		|| S_ISFIFO(stat_buf.st_mode) || S_ISSOCK(stat_buf.st_mode))
	{
		client.response().status(404);
		client.response().body() = read_file(error_path(server).c_str());
	}
	else if (S_ISDIR(stat_buf.st_mode))
	{
		path = index_path(location, path);
		if (!path.empty())
			client.response().body() = read_file(path.c_str());
		else if (location.autoindex())
			client.response().body() = autoindex_page(client, location);
		else
		{
			client.response().status(404);
			client.response().body() = read_file(error_path(server).c_str());
		}
	}
	else
	{
		client.response().body() = read_file(path.c_str());
		if (path.find('.') != std::string::npos)
		{
			std::map<std::string, Location>::iterator	it;

			it = server.config().locations().find(path.substr(path.find_last_of(".")));
			if (it != server.config().locations().end() && it->first != ".bla")
				client.response().body() = cgi_page(client, location, it->second);
		}
	}
}

void	Http::method_head(Server& server, Client& client, Location& location)
{
	std::string	path = get_path(client, location);
	struct stat	stat_buf;

	client.response().status(200);
	client.response().method_head() = true;
	if (stat(path.c_str(), &stat_buf) == -1 || S_ISCHR(stat_buf.st_mode) || S_ISBLK(stat_buf.st_mode) \
		|| S_ISFIFO(stat_buf.st_mode) || S_ISSOCK(stat_buf.st_mode))
		client.response().status(404);
	else if (S_ISDIR(stat_buf.st_mode))
	{
		path = index_path(location, path);
		if (path.empty() && !location.autoindex())
			client.response().status(404);
	}
	(void)server;
}

void	Http::method_put(Server& server, Client& client, Location& location)
{
	std::string	path = get_upload_path(client, location);
	struct stat	stat_buf;

	client.response().status(200);
	if (stat(path.c_str(), &stat_buf))
		client.response().status(201);

	std::ofstream	ofs(path.c_str());

	ofs << client.request().body();
	ofs.close();

	path = get_path(client, location);
	path = index_path(location, path.substr(0, path.find_last_of("/")));
	client.response().body() = read_file(path.c_str());
	client.response().content_location() = client.request().location();
	(void)server;
}

void	Http::method_post(Server& server, Client& client, Location& location)
{
	std::string	path = get_path(client, location);

	client.response().status(200);
	if (path.find('.') != std::string::npos)
	{
		std::map<std::string, Location>::iterator	it;

		it = server.config().locations().find(path.substr(path.find_last_of(".")));
		if (it != server.config().locations().end())
			client.response().body() = cgi_page(client, location, it->second);
	}
	else if (client.request().body().length() > server.config().client_max_body_size())
	{
		client.response().status(413);
		client.response().body() = read_file("error_pages/413.html");
	}
}

void	Http::method_delete(Server& server, Client& client, Location& location)
{
	std::string	path = get_upload_path(client, location);

	if (remove(path.c_str()))
	{
		client.response().status(404);
		client.response().body() = read_file(error_path(server).c_str());
	}
	else
	{
		client.response().status(200);
		path = get_path(client, location);
		path = index_path(location, path.substr(0, path.find_last_of("/")));
		client.response().body() = read_file(path.c_str());
	}
	(void)server;
}

std::string	Http::error_path(Server& server)
{
	std::map<int, std::string>::iterator	it;

	it = server.config().error_pages().find(404);
	if (it != server.config().error_pages().end())
		return it->second;
	return std::string("error_pages/404.html");
}

std::string	Http::index_path(Location& location, std::string path)
{
	std::vector<std::string>::iterator	it;
	struct stat							stat_buf;

	if (*path.rbegin() != '/')
		path.append("/");
	for (it = location.index().begin(); it != location.index().end(); ++it)
	{
		std::string	tmp = path + *it;

		if (stat(tmp.c_str(), &stat_buf) != -1)
			return tmp;
	}
	return std::string("");
}

std::string	Http::autoindex_page(Client& client, Location& location)
{
	std::string				page;
	std::string				path;
	DIR*					dir;
	struct dirent*			entry;
	std::string				dir_path;
	struct stat				stat_buf;
	std::set<std::string>	directories;
	std::set<std::string>	files;

	path = get_path(client, location);
	dir = opendir(path.c_str());
	if (!dir)
		throw std::runtime_error("Error: opendir");
	while (1)
	{
		entry = readdir(dir);
		if (!entry)
			break;
		dir_path = path + "/" + std::string(entry->d_name);
		stat(dir_path.c_str(), &stat_buf);
		if (S_ISDIR(stat_buf.st_mode))
		{
			if (std::string(entry->d_name) != ".")
				directories.insert(entry->d_name);
		}
		else if (S_ISREG(stat_buf.st_mode))
			files.insert(entry->d_name);
	}
	closedir(dir);
	page = "<html>\n<head>\n<title>Index of "+ client.request().location() \
		+ "</title>\n</head>\n<body>\n<h1>Index of "+ client.request().location() \
		+ "</h1>\n<hr>\n<pre>";

	std::set<std::string>::iterator	it;

	for (it = directories.begin(); it != directories.end(); ++it)
		page.append("<a href=\"" + *it + "/\">" + *it + "/</a>\n");
	for (it = files.begin(); it != files.end(); ++it)
		page.append("<a href=\"" + *it + "\">" + *it + "</a>\n");
	page.append("</pre><hr>\n</body>\n</html>\n");
	return page;
}

std::string	Http::cgi_page(Client& client, Location& location, Location& cgi)
{
	std::ifstream		ifs;
	std::ofstream		ofs;
	pid_t				pid;
	int					status;
	std::stringstream	ss;
	std::string			result;
	std::string			file_in = "file_in";

	if (client.request().content_type().find("multipart/form-data") != std::string::npos)
	{
		size_t		begin = client.request().body().find("filename=") + 10;
		size_t		end = client.request().body().find('\"', begin);
		std::string	filename = client.request().body().substr(begin, end - begin);
		
		file_in = get_upload_path(client, location);
		file_in = file_in.erase(file_in.find_last_of('/') + 1).append(filename);
		client.request().body().erase(0, client.request().body().find("\r\n\r\n") + 4);
		client.request().body().erase(client.request().body().find("\r\n------WebKitFormBoundary"));
	}
	ofs.open(file_in.c_str());
	ofs << client.request().body();
	ofs.close();
	pid = fork();
	if (pid < 0)
		throw std::runtime_error("Error: fork");
	if (!pid)
	{
		int			fd_in;
		int			fd_out;
		char*		cwd = getcwd(NULL, 0);
		std::string	path = get_path(client, location);
		std::string	path_file = std::string(cwd) + "/" + path;
		std::string	path_info = "/" + location.upload_pass();
		std::string	path_translated = std::string(cwd) + "/" + location.root() + path_info;

		free(cwd);
		setenv("REQUEST_METHOD", client.request().method().c_str(), 1);
		setenv("SERVER_PROTOCOL", client.request().protocol().c_str(), 1);
		setenv("HTTP_X_Secret_Header_For_Test", client.request().secret_header().c_str(), 1);
		setenv("PATH_INFO", path_info.c_str(), 1);
		setenv("PATH_TRANSLATED", path_translated.c_str(), 1);
		if (!client.request().cookie().empty())
			setenv("QUERY_STRING", cookie_to_query_string(client.request().cookie()).c_str(), 1);
		if (client.request().method() == "GET" && !client.request().query_string().empty())
			setenv("QUERY_STRING", client.request().query_string().c_str(), 1);
		else if (client.request().content_type() == "application/x-www-form-urlencoded" \
			&& !client.request().body().empty())
			setenv("QUERY_STRING", client.request().body().c_str(), 1);
		if (client.request().method() == "POST")
		{
			std::stringstream	ss;

			ss << client.request().body().size();
			if (client.request().content_type().find("multipart/form-data") != std::string::npos)
				setenv("CONTENT_TYPE", "multipart/form-data", 1);
			else
				setenv("CONTENT_TYPE", "application/x-www-form-urlencoded", 1);
			setenv("CONTENT_LENGTH", ss.str().c_str(), 1);
		}
		fd_in = open(file_in.c_str(), O_RDONLY);
		fd_out = open("file_out", O_CREAT | O_WRONLY, 0644);
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
		dup2(fd_out, STDOUT_FILENO);
		close(fd_out);
		exit(execl(cgi.cgi_pass().c_str(), cgi.cgi_pass().c_str(), path_file.c_str(), NULL));
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status))
		throw std::runtime_error("Error: execl");
	ifs.open("file_out");
	ss << ifs.rdbuf();
	result = ss.str().substr(ss.str().find("\r\n\r\n") + 4);
	ifs.close();
	if (client.request().content_type().find("multipart/form-data") == std::string::npos)
		remove(file_in.c_str());
	remove("file_out");
	if (client.request().method() == "GET" && !client.request().query_string().empty())
		client.response().set_cookie(client.request().query_string());
	else if (client.request().content_type() == "application/x-www-form-urlencoded" \
		&& !client.request().body().empty())
		client.response().set_cookie(client.request().body());
	return result;
}

std::string	Http::read_file(const char* path)
{
	std::ifstream		ifs(path);
	std::stringstream	ss;

	if (!ifs.is_open())
		throw std::ios_base::failure("Error: can not open " + std::string(path));
	ss << ifs.rdbuf();
	ifs.close();
	return ss.str();
}

std::string	Http::get_path(Client& client, Location& location)
{
	if (client.request().location() == location.path())
		return location.root();
	else
		return location.root() + "/" + client.request().location().substr(location.path().size());
}

std::string	Http::get_upload_path(Client& client, Location& location)
{
	if (client.request().location() == location.path())
		return location.root() + "/" + location.upload_pass();
	else
		return location.root() + "/" + location.upload_pass() + "/" \
		 + client.request().location().substr(location.path().size());
}

std::string	Http::cookie_to_query_string(std::string& cookie)
{
	size_t		begin = 0;
	size_t		end = 0;
	std::string	result;

	while(begin != std::string::npos)
	{
		if (begin)
			begin += 2;
		end = cookie.find_first_of(';', begin);
		result.append(cookie.substr(begin, end - begin));
		if (end != std::string::npos)
			result.append("&");
		begin = end;
	}
	return result;
}
