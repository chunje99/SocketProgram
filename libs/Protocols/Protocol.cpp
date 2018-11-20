#include "Protocol.h"

Protocol::Protocol(IOSocket& socket)
    : m_socket(socket) 
{}

Protocol::~Protocol()
{}

ProtocolS::ProtocolS(IOSocketS& socketS)
    : m_socketS(socketS) 
{}

ProtocolS::~ProtocolS()
{}
