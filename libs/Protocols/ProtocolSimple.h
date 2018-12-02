#ifndef PROTOCOL_SIMPLE_H
#define PROTOCOL_SIMPLE_H

#include "Protocol.h"

class ProtocolSimple: public Protocol
{
    public:
        using Protocol::Protocol;
        void sendMessage(std::string const& message, int timeout=-1) override;
        void recvMessage(std::string & message, int timeout=-1) override;
        void startProtocol() override;
};
#endif
