
#include "package.h"

namespace package
{
	//convert size_t to string
	std::string to_str(Package::size_t l)
	{
		char* cast = reinterpret_cast<char*>(&l);
		std::string str;
		for (int i = 0; i < Package::headSize; ++i)
		{
			str += cast[i];
		}
		return str;
	}
	//convert string to size_t
	Package::size_t to_u32(const std::string& s)
	{
		if (s.size() != Package::headSize)
		{
			throw std::logic_error("the length of str must be headSize");
		}
		char cast[Package::headSize];
		for (int i = 0; i < Package::headSize; ++i)
		{
			cast[i] = s[i];
		}
		Package::size_t* p = reinterpret_cast<Package::size_t*>(cast);
		auto length = *p;
		return length;
	}

	std::string Package::pack(const std::string & rawData)
	{
		this->rawData = rawData;
		size_t length = rawData.size();

		auto head = to_str(length);

		this->data = head + rawData;
		return this->data;
	}

	std::string Package::unpack(const std::string & data)
	{
		this->data = data;
		
		auto length = to_u32(data.substr(0,4));

		auto temp = data;
		this->rawData = temp.erase(0, Package::headSize).substr(0, length);
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

	Package::size_t Package::size() const
	{
		return this->rawData.size();
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

	Transceiver::Transceiver()
	{
		this->s = nullptr;
		this->buffer.clear();
		this->lastFinished = false;
	}

	void Transceiver::bindSocket(socket & s)
	{
		this->s = &s;
	}

	void Transceiver::send(const Package & p)
	{
		try
		{
			if (!(this->s))
			{
				throw std::logic_error("socket not binded");
			}
			packageSend(*(this->s), p);

		}
		catch (std::exception e)
		{
			std::cerr << __func__ << e.what() << std::endl;
		}
	}

	void Transceiver::receive(Package & p)
	{
		try
		{
			if (!s)
			{
				throw std::logic_error("socket not binded");
			}
			//this->buffer have fully received package
			if (buffer.size() > 1 || lastFinished)
			{
				//return the first package in the buffer
				p.unpack(*(buffer.begin()));
				buffer.erase(buffer.begin());
				if (buffer.size() == 0)
				{
					lastFinished = false;
				}
			}
			//no fully received package in buffer
			else
			{
				static constexpr size_t bufSize = 10 * Package::maxSize;
				char buf[bufSize];
				asio::error_code errc;
				//length of message read from socket
				auto msgLen = s->read_some(asio::buffer(buf), errc);
				//the length of used data in buf
				Package::size_t usedLen = 0;
				//read till msgLen >= len
				auto readTillLen = [&](size_t len)
				{
					while (msgLen < len)
					{
						char tempBuf[bufSize];
						auto tempLen = s->read_some(asio::buffer(tempBuf), errc);;
						memcpy(buf + msgLen, tempBuf, tempLen);
						msgLen += tempLen;
					}
				};
				//a unfinished package in buffer
				if (buffer.size() == 1)
				{
					auto strInBuffer = *buffer.begin();
					//data too short, could not deduce the length of package 
					if (strInBuffer.size() < Package::headSize
						&& msgLen < Package::headSize - strInBuffer.size())
					{
						readTillLen(Package::headSize - strInBuffer.size());
					}
					//notice the buffer str may less than headSize
					auto pakLen = Package::headSize + (strInBuffer.size() >= Package::headSize ?
						to_u32(strInBuffer.substr(0, Package::headSize)) :
						to_u32((strInBuffer
							+ std::string(buf, Package::headSize - strInBuffer.size()))));
					auto lenToRead = pakLen - strInBuffer.size();
					usedLen = lenToRead;
					//read whole package
					readTillLen(lenToRead);
					p.unpack(strInBuffer + std::string(buf, lenToRead));
					//erase data in buffer
					buffer.erase(buffer.begin());
					lastFinished = false;
				}
				//no package in buffer
				else
				{
					//ensure enough data to deduce the length
					readTillLen(Package::headSize);
					auto pakLen = Package::headSize + to_u32(std::string(buf, 4));
					usedLen = pakLen;
					readTillLen(pakLen);
					p.unpack(std::string(buf, pakLen));
				}
				//there are rest data in buf, needed to load into buffer
				while (usedLen < msgLen)
				{
					if (msgLen - usedLen < Package::headSize)
					{
						buffer.push_back(std::string(buf + usedLen, msgLen - usedLen));
						lastFinished = false;
						usedLen = msgLen;
					}
					else
					{
						auto pakLen = Package::headSize + to_u32(std::string(buf + usedLen, 4));
						if (usedLen + pakLen < msgLen)
						{
							buffer.push_back(std::string(buf + usedLen, pakLen));
							usedLen += pakLen;
						}
						else
						{
							buffer.push_back(std::string(buf + usedLen, msgLen - usedLen));
							lastFinished = true;
							usedLen = msgLen;
						}
					}
				}
				//if (buffer.size() > 0 && (*buffer.end()).size() > 0 && (*buffer.end())[1] != '/0')
				//{
				//	std::cerr << 'e' << std::endl;
				//}
			}
		}
		catch (std::exception &e)
		{
			std::cerr << __func__ << e.what() << std::endl;
		}
	}
};

