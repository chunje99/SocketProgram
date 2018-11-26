#include "Protocol.h"

Protocol::Protocol(IOSocket& socket)
    : m_socket(socket) 
{}

Protocol::~Protocol()
{}
