#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>

class IOSocket;
class IOSocketS;
class Protocol
{
    protected:
        IOSocket&   m_socket;
    public:
        Protocol(IOSocket& socket);
        ~Protocol();

        virtual void sendMessage(std::string const& message) = 0;
        virtual void recvMessage(std::string & message) = 0;
        virtual void startProtocol() = 0;
};

#endif
