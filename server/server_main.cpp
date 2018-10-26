//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE
#define _WIN32_WINNT 0x0501
#define _CRT_SECURE_NO_WARNINGS

#include <ctime>
#include <iostream>
#include <string>
#include <chrono>
#include <asio.hpp>

using asio::ip::tcp;

std::string make_daytime_string()
{
	using namespace std::chrono; // For time_t, time and ctime;
	auto time = system_clock::to_time_t(system_clock::now());
	return std::ctime(&time);
}

int main()
{
	try
	{
		asio::io_service io_service;

		tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 13));

		for (;;)
		{
			tcp::socket socket(io_service);
			acceptor.accept(socket);

			std::string message = make_daytime_string();

			asio::error_code ignored_error;
			asio::write(socket, asio::buffer(message), ignored_error);

			std::cout << "message sent:" << message << std::endl;
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}