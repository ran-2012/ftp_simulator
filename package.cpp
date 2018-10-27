
#include "package.h"

namespace package
{
	std::string Package::pack(const std::string & rawData)
	{
		this->rawData = rawData;
		size_t length = rawData.size();
		//convert size_t to char
		char* cast = reinterpret_cast<char*>(&length);
		std::string head;
		for (int i = 0; i < sizeof(size_t); ++i)
		{
			head += cast[i];
		}

		this->data = head + rawData;
		return this->data;
	}

	std::string Package::unpack(const std::string & data)
	{
		this->data = data;
		//convert char to size_t
		char cast[sizeof(size_t)];
		for (int i = 0; i < sizeof(size_t); ++i)
		{
			cast[i] = data[i];
		}
		size_t length = reinterpret_cast<size_t>(cast);

		auto temp = data;
		this->rawData = temp.erase(0, sizeof(size_t)).substr(0, length);
		return this->rawData;
	}

	std::string Package::getRawData() const
	{
		return this->rawData;
	}

	std::string Package::getData() const
	{
		return this->data;
	}

	void packageSend(socket & s, const Package & p)
	{
		asio::error_code errc;
		asio::write(s, asio::buffer(p.getData()), errc);
	}

	void packageReceive(socket & s, Package & p)
	{
		char buffer[Package::maxSize];
		asio::error_code errc;
		auto len = s.read_some(asio::buffer(buffer), errc);
		p.unpack(std::string(buffer, len));
	}
};

