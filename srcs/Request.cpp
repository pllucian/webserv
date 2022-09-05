#include "Request.hpp"

Request::Request(void) : _buffer(""), _method(""), _location(""), _query_string(""), \
	_protocol(""), _host(""), _connection(""), _accept(""), _content_type(""), \
	_content_length(0), _transfer_encoding(""), _secret_header("0"), _cookie(""), \
	_body(""),	_ready(false), _error(false) {}

Request::Request(const Request& src)
{
	*this = src;
}

Request::~Request() {}

Request&	Request::operator=(const Request& rhs)
{
	if (this != &rhs)
	{
		_buffer = rhs._buffer;
		_method = rhs._method;
		_location = rhs._location;
		_query_string = rhs._query_string;
		_protocol = rhs._protocol;
		_host = rhs._host;
		_connection = rhs._connection;
		_accept = rhs._accept;
		_content_type = rhs._content_type;
		_content_length = rhs._content_length;
		_transfer_encoding = rhs._transfer_encoding;
		_secret_header = rhs._secret_header;
		_cookie = rhs._cookie;
		_body = rhs._body;
		_ready = rhs._ready;
		_error = rhs._error;
	}
	return *this;
}

std::string&	Request::buffer(void)
{
	return _buffer;
}

std::string&	Request::method(void)
{
	return _method;
}

std::string&	Request::location(void)
{
	return _location;
}

std::string&	Request::query_string(void)
{
	return _query_string;
}

std::string&	Request::protocol(void)
{
	return _protocol;
}

std::string&	Request::host(void)
{
	return _host;
}

std::string&	Request::connection(void)
{
	return _connection;
}

std::string&	Request::accept(void)
{
	return _accept;
}

std::string&	Request::content_type(void)
{
	return _content_type;
}

size_t&	Request::content_length(void)
{
	return _content_length;
}

std::string&	Request::transfer_encoding(void)
{
	return _transfer_encoding;
}

std::string&	Request::secret_header(void)
{
	return _secret_header;
}

std::string&	Request::cookie(void)
{
	return _cookie;
}

std::string&	Request::body(void)
{
	return _body;
}

bool	Request::ready(void)
{
	return _ready;
}

bool	Request::error(void)
{
	return _error;
}

void	Request::reset(void)
{
	_method = "";
	_location = "";
	_query_string = "";
	_protocol = "";
	_host = "";
	_connection = "";
	_accept = "";
	_content_type = "";
	_content_length = 0;
	_transfer_encoding = "";
	_secret_header = "0";
	_cookie = "";
	_body = "";
	_ready = false;
	_error = false;
}

void	Request::parse_buffer(void)
{
	if (_method.empty())
	{
		size_t	pos = _buffer.find("\r\n\r\n");

		if (pos == std::string::npos)
			return;

		std::string	headers = _buffer.substr(0, pos + 4);

		parse_headers(headers);
		_buffer.erase(0, pos + 4);
	}
	if (_content_length)
		parse_body_length();
	else if (_transfer_encoding == "chunked")
		parse_body_chunked();
}

void	Request::parse_headers(std::string& chunk)
{
	size_t		count_line = 0;
	size_t		begin = 0;
	size_t		end = 0;
	std::string	line;

	while (chunk[begin])
	{
		end = chunk.find("\r\n", begin);
		line = chunk.substr(begin, end - begin);
		if (!count_line)
			parse_start_line(line);
		else if (count_line == 1)
			parse_host(line);
		else
			parse_line(line);
		++count_line;
		begin = end + 2;
	}
	if (!_content_length && _transfer_encoding != "chunked")
		_ready = true;
}

void	Request::parse_start_line(std::string& line)
{
	size_t	begin = 0;
	size_t	end = 0;

	end = line.find_first_of(" ", begin);
	_method = line.substr(begin, end - begin);
	begin = end + 1;
	end = line.find_first_of(" ?", begin);
	_location = line.substr(begin, end - begin);
	begin = end + 1;
	if (line[end] == '?')
	{
		end = line.find_first_of(" ", begin);
		_query_string = line.substr(begin, end - begin);
		begin = end + 1;
	}
	_protocol = line.substr(begin);
	if (!(_protocol == "HTTP/1.1" || _protocol == "HTTP/1.0"))
		_error = true;
}

void	Request::parse_host(std::string& line)
{
	size_t		begin = 0;
	size_t		end = 0;
	std::string	header;

	end = line.find_first_of(" ", begin);
	header = line.substr(begin, end - begin);
	for (size_t i = 0; i < header.size(); ++i)
		header[i] = std::tolower(header[i]);
	if (header == "host:")
		_host = line.substr(end + 1);
	else
		_error = true;
}

void	Request::parse_line(std::string& line)
{
	size_t		begin = 0;
	size_t		end = 0;
	std::string	header;

	end = line.find_first_of(" ", begin);
	header = line.substr(begin, end - begin);
	for (size_t i = 0; i < header.size(); ++i)
		header[i] = std::tolower(header[i]);
	if (header == "host:")
		_error = true;
	else if (header == "connection:")
		_connection = line.substr(end + 1);
	else if (header == "accept:")
		_accept = line.substr(end + 1);
	else if (header == "content-type:")
		_content_type = line.substr(end + 1);
	else if (header == "content-length:")
		_content_length = strtoul(line.substr(end + 1).c_str(), NULL, 10);
	else if (header == "transfer-encoding:")
		_transfer_encoding = line.substr(end + 1);
	else if (header == "x-secret-header-for-test:")
		_secret_header = line.substr(end + 1);
	else if (header == "cookie:")
		_cookie = line.substr(end + 1);
}

void	Request::parse_body_length(void)
{
	if (_body.size() + _buffer.size() <= _content_length)
	{
		_body.append(_buffer);
		_buffer.clear();
	}
	else
	{
		_body.append(_buffer.substr(0, _content_length - _body.size()));
		_buffer.erase(0, _content_length - _body.size());
	}
	if (_body.size() == _content_length)
		_ready = true;
}

void	Request::parse_body_chunked(void)
{
	size_t		begin;
	size_t		end;
	size_t		chunk_size = 0;
	std::string	chunk_data = "";

	while (!_ready)
	{
		begin = 0;
		end = 0;
		end = _buffer.find("\r\n", begin);
		if (end == std::string::npos)
			break;
		chunk_size = strtoul(_buffer.substr(begin, end - begin).c_str(), NULL, 16);
		end += 2;
		begin = end;
		chunk_data.clear();
		while (chunk_data.size() < chunk_size)
		{
			end = _buffer.find("\r\n", begin);
			if (end == std::string::npos)
				return;
			chunk_data.append(_buffer.substr(begin, end - begin));
			if (chunk_data.size() < chunk_size)
				chunk_data.append("\r\n");
			end += 2;
			begin = end;
		}
		_body.append(chunk_data);
		_buffer.erase(0, begin);
		if (!chunk_size)
			_ready = true;
	}
}
