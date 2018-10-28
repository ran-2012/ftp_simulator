
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
#include <filesystem>
#include "../package.h"

#define DIRECTORY "server_files"
#define PORT 20021

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
				try
				{
					//asio::deadline_timer t(io_service, boost::posix_time::seconds(1));
					//t.wait();
					tran.receive(p);
					input = p.getRawData();
					std::cout << input << std::endl;
					
					cmd = input.substr(0, input.find(' '));
					if (cmd == "list")
					{
						using namespace std::experimental::filesystem;
						int count = 0;
						for (auto& fi : directory_iterator(DIRECTORY))
						{
							++count;
						}
						std::string strn(' ', 1);
						strn[0] = count;
						p.pack(strn);
						tran.send(p);
						for (auto& fi : directory_iterator(DIRECTORY))
						{
							p.pack(fi.path().filename().string());
							tran.send(p);
						}
					}
					if (cmd == "get")
					{
						using namespace std::experimental::filesystem;
						std::string fileName;
						std::stringstream ss;

						tran.receive(p);

						fileName = p.getRawData();
						fileName = '/' + fileName;
						fileName = DIRECTORY + fileName;

						std::cout << fileName << std::endl;
						auto size =  file_size(fileName);
						std::ifstream ifs;
						ifs.open(fileName, std::ios_base::binary);

						ss << size;
						p.pack(ss.str());
						tran.send(p);

						const auto bufSize = package::Package::maxBodySize;
						char buf[bufSize];
						while (size > bufSize)
						{
							ifs.read(buf, bufSize);
							p.pack(buf);
							tran.send(p);
							size -= bufSize;
						}

						ifs.read(buf, size);
						p.pack(std::string(buf, size));
						tran.send(p);

						ifs.close();
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