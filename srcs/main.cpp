#include "WebServ.hpp"

int	main(int argc, char** argv)
{
	std::string	path = "configs/default";

	if (argc > 2)
	{
		std::cerr << "Error: too many arguments\n";
		return 1;
	}
	if (argc > 1)
		path = argv[1];

	WebServ	webserv(path);

	try
	{
		webserv.start();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return 0;
}
