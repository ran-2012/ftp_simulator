
#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE
#define _WIN32_WINNT 0x0501
#define _CRT_SECURE_NO_WARNINGS

#include <ctime>
#include <string>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iostream>
#include <exception>

#include <asio.hpp>
#include <experimental/filesystem>
#include "package.h"

#define DIRECTORY "server_files"
#define PORT 60006

int main()
{
	using asio::ip::tcp;

	try
	{
		asio::io_service io_service;

		tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), PORT));

		while(1)
		{
			//establish connection
			tcp::socket socket(io_service);
			acceptor.accept(socket);

			package::Transceiver tran;
			tran.bindSocket(socket);

			asio::error_code ignored_error;
			package::Package p;
			std::string input;
			std::string cmd;

			p.pack("server connected\n");
			tran.send(p);
			//send message loop
			while (1)
			{
				//asio::deadline_timer t(io_service, boost::posix_time::seconds(1));
				//t.wait();
				
				tran.receive(p);
				input = p.getRawData();
				std::cout << input << std::endl;
				tran.send(p);
			}
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}