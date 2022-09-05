#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <string>
# include <set>
# include <vector>

class Location
{

	public:
		Location(void);
		Location(const std::string& path);
		Location(const Location& src);
		~Location();

		Location&	operator=(const Location& rhs);

		std::string&					path(void);
		std::set<std::string>&			methods(void);
		std::pair<int, std::string>&	return_(void);
		std::string&					root(void);
		bool&							autoindex(void);
		std::vector<std::string>&		index(void);
		std::string&					cgi_pass(void);
		std::string&					upload_pass(void);

	private:
		std::string					_path;
		std::set<std::string>		_methods;
		std::pair<int, std::string>	_return;
		std::string					_root;
		bool						_autoindex;
		std::vector<std::string>	_index;
		std::string					_cgi_pass;
		std::string					_upload_pass;
};

#endif
