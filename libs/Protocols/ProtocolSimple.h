#ifndef PROTOCOL_SIMPLE_H
#define PROTOCOL_SIMPLE_H

#include "Protocol.h"

class ProtocolSimple: public Protocol
{
    public:
        using Protocol::Protocol;
        void sendMessage(std::string const& message) override;
        void recvMessage(std::string & message) override;
        void startProtocol() override;
};
#endif
