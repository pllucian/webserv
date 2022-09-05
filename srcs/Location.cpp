#include "Location.hpp"

Location::Location(void) : _path(""), _return(std::make_pair(0, "")), _root("www"), \
	_autoindex(false), _cgi_pass(""), _upload_pass("") {}

Location::Location(const std::string& path) : _path(path), _return(std::make_pair(0, "")), _root("www"), \
	_autoindex(false), _cgi_pass(""), _upload_pass("") {}

Location::Location(const Location& src)
{
	*this = src;
}

Location::~Location() {}

Location&	Location::operator=(const Location& rhs)
{
	if (this != &rhs)
	{
		_path = rhs._path;
		_methods = rhs._methods;
		_return = rhs._return;
		_root = rhs._root;
		_autoindex = rhs._autoindex;
		_index = rhs._index;
		_cgi_pass = rhs._cgi_pass;
		_upload_pass = rhs._upload_pass;
	}
	return *this;
}

std::string&	Location::path(void)
{
	return _path;
}

std::set<std::string>&	Location::methods(void)
{
	return _methods;
}

std::pair<int, std::string>&	Location::return_(void)
{
	return _return;
}

std::string&	Location::root(void)
{
	return _root;
}

bool&	Location::autoindex(void)
{
	return _autoindex;
}

std::vector<std::string>&	Location::index(void)
{
	return _index;
}

std::string&	Location::cgi_pass(void)
{
	return _cgi_pass;
}

std::string&	Location::upload_pass(void)
{
	return _upload_pass;
}
