#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>
# include <cstdlib>

class Request
{
	public:
		Request(void);
		Request(const Request& src);
		~Request();

		Request&	operator=(const Request& rhs);

		std::string&	buffer(void);
		std::string&	method(void);
		std::string&	location(void);
		std::string&	query_string(void);
		std::string&	protocol(void);
		std::string&	host(void);
		std::string&	connection(void);
		std::string&	accept(void);
		std::string&	content_type(void);
		size_t&			content_length(void);
		std::string&	transfer_encoding(void);
		std::string&	secret_header(void);
		std::string&	cookie(void);
		std::string&	body(void);
		bool			ready(void);
		bool			error(void);
		void			reset(void);
		void			parse_buffer(void);

	private:
		std::string	_buffer;
		std::string	_method;
		std::string	_location;
		std::string	_query_string;
		std::string	_protocol;
		std::string	_host;
		std::string	_connection;
		std::string	_accept;
		std::string	_content_type;
		size_t		_content_length;
		std::string	_transfer_encoding;
		std::string	_secret_header;
		std::string	_cookie;
		std::string	_body;
		bool		_ready;
		bool		_error;

		void	parse_headers(std::string& headers);
		void	parse_start_line(std::string& line);
		void	parse_host(std::string& line);
		void	parse_line(std::string& line);
		void	parse_body_length(void);
		void	parse_body_chunked(void);
};

#endif
