#pragma once

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
		typedef uint32_t size_t;
		std::string rawData;
		std::string data;
	public:
		static constexpr size_t maxSize = 1024;//bytes
		//pack length of rawData into data, return data
		std::string pack(const std::string& rawData);
		//unpack data, return rawData
		std::string unpack(const std::string& data);
		std::string getRawData()const;
		std::string getData()const;
	};

	void packageSend(socket& s, const Package& p);
	void packageReceive(socket& s, Package& p);

};
