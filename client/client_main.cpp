//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

int main(int argc, char* argv[])
{
	using asio::ip::tcp;
	try
	{

		asio::io_service io_service;

		tcp::resolver resolver(io_service);
		tcp::resolver::query query("127.0.0.1", "daytime");
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);


		for (;;)
		{
			asio::deadline_timer t(io_service, boost::posix_time::seconds(1));
			t.wait();

			tcp::socket socket(io_service);
			asio::connect(socket, endpoint_iterator); 

			char buf[256];
			asio::error_code error;

			size_t len = socket.read_some(asio::buffer(buf), error);

			if (error == asio::error::eof)
				;//break; // Connection closed cleanly by peer.
			else if (error)
				throw asio::system_error(error); // Some other error.
			else
				std::cout << "message received:";
				std::cout.write(buf, len);
				std::cout << std::endl;
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}