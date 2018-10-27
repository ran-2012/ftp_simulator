
#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE
#define _WIN32_WINNT 0x0501
#define _CRT_SECURE_NO_WARNINGS

#include <ctime>
#include <string>
#include <chrono>
#include <iostream>
#include <exception>

#include <asio.hpp>
#include <filesystem>
#include "../package.h"

#define PORT 60000

std::string make_daytime_string()
{
	using namespace std::chrono; 
	auto time = system_clock::to_time_t(system_clock::now());
	return std::ctime(&time);
}

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

			asio::error_code ignored_error;
			//filesystem test
			using namespace std::experimental::filesystem;
			for (auto& fi : directory_iterator("files"))
			{
				std::string message =fi.path().string();
				std::cout << message << std::endl;
				asio::write(socket, asio::buffer(message), ignored_error);
			}
			//send message loop
			while (1)
			{
				try
				{
					asio::deadline_timer t(io_service, boost::posix_time::seconds(1));
					t.wait();

					package::Package p;
					std::string message = make_daytime_string();
					p.pack(message);
					package::packageSend(socket, p);

					//asio::write(socket, asio::buffer(message), ignored_error);

					std::cout << "message sent:" << message << std::endl;
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