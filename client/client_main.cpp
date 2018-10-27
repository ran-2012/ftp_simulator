
#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE
#define _WIN32_WINNT 0x0501
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

#include <asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../package.h"

int main(int argc, char* argv[])
{
	using asio::ip::tcp;
	try
	{

		asio::io_service io_service;

		tcp::resolver resolver(io_service);
		tcp::resolver::query query("127.0.0.1", "60000");
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);


		while(1)
		{
			//connection
			tcp::socket socket(io_service);
			asio::connect(socket, endpoint_iterator); 

			while (1)
			{
				try 
				{
					asio::error_code error;

					package::Package p;

					package::packageReceive(socket, p);

					if (error == asio::error::eof)
						;//break; // Connection closed cleanly by peer.
					else if (error)
						throw asio::system_error(error); // Some other error.
					else
					{
						std::cout << "message received:";
						std::cout << p.getRawData();
						std::cout << std::endl;
					}

				}
				catch (std::exception e)
				{
					std::cerr << e.what() << std::endl;
					break;
				}
			}
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}