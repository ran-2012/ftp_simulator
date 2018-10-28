
#define BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE
#define _WIN32_WINNT 0x0501
#define _CRT_SECURE_NO_WARNINGS

#include <fstream>
#include <sstream>
#include <iostream>

#include <asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../package.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT "20021"
#define DIRECTORY "client_files"

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
			
			package::Transceiver tran;
			tran.bindSocket(socket);

			asio::error_code error;
			package::Package p;
			std::string input;
			std::string cmd;

			tran.receive(p);
			std::cout << p.getRawData() << std::endl;
			//main loop
			while (1)
			{
				try 
				{
					std::cout << '>';
					std::getline(std::cin, input);

					p.pack(input);
					tran.send(p);

					cmd = input.substr(0, input.find(' '));
					if (cmd == "list")
					{
						tran.receive(p);
						std::string strn = p.getRawData();
						//the number of files
						int n = strn[0];
						std::cout << n << " files in server" << std::endl;
						for (int i = 0; i < n; ++i)
						{
							tran.receive(p);
							std::cout << '\t' << p.getRawData() << std::endl;
						}
					}
					else if (cmd == "get")
					{
						std::string fileName = input.erase(0, input.find(' ') + 1);
						std::stringstream ss;

						p.pack(fileName);
						tran.send(p);

						fileName = '/' + fileName;
						fileName = DIRECTORY + fileName;

						std::ofstream ofs;
						ofs.open(fileName, std::ios_base::binary);

						tran.receive(p);
						std::string ssize = p.getRawData();
						size_t size;
						ss << ssize;
						ss >> size;

						std::cout << fileName << " file size : " << size << std::endl;
						
						const auto bufSize = package::Package::maxBodySize;
						while (size > bufSize)
						{
							tran.receive(p);
							ofs.write(p.getRawData().c_str(), bufSize);
							size -= bufSize;
						}

						tran.receive(p);
						ofs.write(p.getRawData().c_str(), size);
						ofs.close();
					}
					else
					{
						std::cout << "Unknown command" << std::endl;
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