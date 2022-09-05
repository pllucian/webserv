#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <sstream>
# include <fstream>
# include <string>
# include <set>
# include <cstring>
# include <cstdlib>

class Response
{
	public:
		Response(void);
		Response(const Response& src);
		~Response();

		Response&	operator=(const Response& rhs);

		std::string&			buffer(void);
		std::string&			protocol(void);
		std::string&			status(void);
		void					status(int code);
		std::string&			server(void);
		std::string&			date(void);
		std::string&			connection(void);
		std::string&			location(void);
		std::set<std::string>&	set_cookie(void);
		void					set_cookie(std::string cookie);
		std::string&			content_location(void);
		std::string&			content_type(void);
		std::string&			content_length(void);
		void					content_length(size_t length);
		std::string&			body(void);
		bool&					method_head(void);
		void					reset(void);
		void					add_buffer(void);

	private:
		std::string				_buffer;
		std::string				_protocol;
		std::string				_status;
		std::string				_server;
		std::string				_date;
		std::string				_connection;
		std::string				_location;
		std::set<std::string>	_set_cookie;
		std::string				_content_location;
		std::string				_content_type;
		std::string				_content_length;
		std::string				_body;
		bool					_method_head;
};

#endif
