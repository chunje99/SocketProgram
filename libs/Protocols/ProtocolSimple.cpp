#include "ProtocolSimple.h"
#include "Socket.h"
#include "iostream"

void ProtocolSimple::sendMessage(std::string const& message)
{
    m_socket.putMessage(message.c_str(), message.size());
}

void ProtocolSimple::startProtocol()
{
}

class StringSizer
{
    private:
        std::string&    stringData;
        std::size_t&    currentSize;
    public:
        StringSizer(std::string& stringData, std::size_t& currentSize)
            : stringData(stringData)
              , currentSize(currentSize)
    {
        stringData.resize(stringData.capacity());
    }
        ~StringSizer()
        {
            stringData.resize(currentSize);
        }
        void incrementSize(std::size_t amount)
        {
            currentSize += amount;
        }
};

void ProtocolSimple::recvMessage(std::string & message)
{
    std::size_t dataRead = 0;
    message.clear();

    while(true){
        StringSizer stringSizer(message, dataRead);
        std::size_t const dataMax = message.capacity() -1;
        char* buffer = &message[0];
        std::size_t got = m_socket.getMessage(buffer + dataRead, dataMax - dataRead, [](std::size_t,const char* str){return true;});
        std::cout.write(buffer+dataRead, got);
        dataRead += got;
        if(got == 0)
            break;
        message.reserve(message.capacity()*1.5 + 10);
    }
}
