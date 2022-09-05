#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Request.hpp"
# include "Response.hpp"

class Client
{
	public:
		Client(void);
		Client(int fd, std::string address);
		Client(const Client& src);
		~Client();

		Client&	operator=(const Client& rhs);

		int				fd(void);
		std::string&	address(void);
		Request&		request(void);
		Response&		response(void);

	private:
		int			_fd;
		std::string	_address;
		Request		_request;
		Response	_response;
};

#endif
