#pragma once

#include <list>
#include <utility>
#include <ctime>
#include <string>
#include <chrono>
#include <iostream>
#include <exception>

#include <asio.hpp>

namespace package
{
	using socket = asio::ip::tcp::socket;
	//base class Package designed for pack data length and data
	class Package
	{
	protected:
		std::string rawData;
		std::string data;
	public:
		typedef uint32_t size_t;
		static constexpr size_t maxBodySize = 1024;//bytes
		static constexpr size_t headSize = 4;//bytes
		static constexpr size_t maxSize = headSize + maxBodySize;//bytes
		//pack length of rawData into data, return data
		std::string pack(const std::string& rawData);
		//unpack data, return rawData
		std::string unpack(const std::string& data);
		std::string getRawData()const;
		std::string getData()const;
		size_t size()const;
	};
	
	class Transceiver
	{
		//a receive opreation may read more than a package, use buffer to store next part of package
		std::list<std::string> buffer;
		//if the last package in buffer is fully received
		bool lastFinished;
		socket* s;
	public:
		Transceiver();
		void bindSocket(socket& s);
		void send(const Package& p);
		void receive(Package& p);
	};

	void packageSend(socket& s, const Package& p);
	void packageReceive(socket& s, Package& p);

};
