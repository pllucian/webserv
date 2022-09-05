#include "Response.hpp"

Response::Response(void) : _buffer(""), _protocol("HTTP/1.1"), _status(""), \
	_server("webserv"), _date(""), _connection("close"), _location(""), \
	_content_location(""), _content_type(""), _content_length(""), _body(""), \
	_method_head(false) {}

Response::Response(const Response& src)
{
	*this = src;
}

Response::~Response() {}

Response&	Response::operator=(const Response& rhs)
{
	if (this != &rhs)
	{
		_buffer = rhs._buffer;
		_protocol = rhs._protocol;
		_status = rhs._status;
		_server = rhs._server;
		_date = rhs._date;
		_connection = rhs._connection;
		_location = rhs._location;
		_set_cookie = rhs._set_cookie;
		_content_location = rhs._content_location;
		_content_type = rhs._content_type;
		_content_length = rhs._content_length;
		_body = rhs._body;
		_method_head = rhs._method_head;
	}
	return *this;
}

std::string&	Response::buffer(void)
{
	return _buffer;
}

std::string&	Response::protocol(void)
{
	return _protocol;
}

std::string&	Response::status(void)
{
	return _status;
}

void	Response::status(int code)
{
	std::stringstream	ss;

	ss << code << " ";
	switch (code)
	{
		case 200:
			ss << "OK";
			break;
		case 201:
			ss << "Created";
			break;
		case 301:
			ss << "Moved Permanently";
			break;
		case 400:
			ss << "Bad Request";
			break;
		case 404:
			ss << "Not Found";
			break;
		case 405:
			ss << "Method Not Allowed";
			break;
		case 413:
			ss << "Payload Too Large";
			break;
		case 505:
			ss << "HTTP Version Not Supported";
			break;
		default:
			ss << "Unknown code";
			break;
	}
	_status = ss.str();
	_content_type = "*/*";
}

std::string&	Response::server(void)
{
	return _server;
}

std::string&	Response::date(void)
{
	return _date;
}

std::string&	Response::connection(void)
{
	return _connection;
}

std::string&	Response::location(void)
{
	return _location;
}

std::set<std::string>&	Response::set_cookie(void)
{
	return _set_cookie;
}

void	Response::set_cookie(std::string cookie)
{
	size_t	begin = 0;
	size_t	end = 0;

	while(begin != std::string::npos)
	{
		if (begin)
			++begin;
		end = cookie.find_first_of('&', begin);
		_set_cookie.insert(cookie.substr(begin, end - begin));
		begin = end;
	}
}

std::string&	Response::content_location(void)
{
	return _content_location;
}

std::string&	Response::content_type(void)
{
	return _content_type;
}

std::string&	Response::content_length(void)
{
	return _content_length;
}

void	Response::content_length(size_t length)
{
	std::stringstream	ss;

	ss << length;
	_content_length = ss.str();
}

std::string&	Response::body(void)
{
	return _body;
}

bool&	Response::method_head(void)
{
	return _method_head;
}

void	Response::reset(void)
{
	_protocol = "HTTP/1.1";
	_status = "";
	_server = "webserv";
	_date = "";
	_connection = "close";
	_location = "";
	_set_cookie.clear();
	_content_location = "";
	_content_type = "";
	_content_length = "";
	_body = "";
	_method_head = false;
}

void	Response::add_buffer(void)
{
	time_t	timer;

	time(&timer);
	_date = std::string(ctime(&timer));
	_date.erase(_date.end() - 1);
	_buffer.append(_protocol + " " + _status + "\r\n");
	_buffer.append("Server: " + _server + "\r\n");
	_buffer.append("Date: " + _date + "\r\n");
	_buffer.append("Connection: " + _connection + "\r\n");
	if (!_location.empty())
		_buffer.append("Location: " + _location + "\r\n");
	if (!_set_cookie.empty())
	{
		std::set<std::string>::iterator	it;

		for (it = _set_cookie.begin(); it != _set_cookie.end(); ++it)
			_buffer.append("Set-Cookie: " + *it + "\r\n");
	}
	if (!_content_location.empty())
		_buffer.append("Content-Location: " + _content_location + "\r\n");
	if (!_content_type.empty())
	{
		_buffer.append("Content-Type: " + _content_type + "\r\n");
		content_length(_body.size());
		_buffer.append("Content-Length: " + _content_length + "\r\n");
	}
	_buffer.append("\r\n");
	if (!_method_head)
		_buffer.append(_body + "\r\n");
}
