
#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE
#define _WIN32_WINNT 0x0501
#define _CRT_SECURE_NO_WARNINGS

#include <fstream>
#include <sstream>
#include <iostream>

#include <asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../package.h"

#define SERVER_IP "192.168.43.75"
#define SERVER_PORT "60006"
#define DIRECTORY "client_files"

int main(int argc, char* argv[])
{
	using asio::ip::tcp;
	try
	{
		asio::io_service io_service;

		tcp::resolver resolver(io_service);
		tcp::resolver::query query(SERVER_IP, SERVER_PORT);
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

		while(1)
		{
			try
			{
				tcp::socket socket(io_service);
				asio::connect(socket, endpoint_iterator);

				package::Transceiver tran;
				tran.bindSocket(socket);

				asio::error_code error;
				package::Package p;
				std::string input;
				std::string cmd;

				tran.receive(p);
				std::cout << p.getRawData() << std::endl;

				while (std::cin)
				{
					std::string s;
					std::getline(std::cin, s);
					p.pack(s);
					tran.send(p);

					tran.receive(p);
					std::cout << p.getRawData() << std::endl;
				}
			}
			catch (std::exception &e)
			{
				std::cerr << e.what() << std::endl;
			}
			//connection
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}