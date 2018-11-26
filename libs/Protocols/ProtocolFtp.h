#ifndef PROTOCOL_Ftp_H
#define PROTOCOL_Ftp_H

#include "Protocol.h"
#include <vector>
#include "fstream"

class ConnectSocket;
class ProtocolFtp: public Protocol
{
    public:
        using Protocol::Protocol;
        ProtocolFtp(IOSocket& socket);
        ~ProtocolFtp();
        void sendMessage(std::string const& message) override;
        void recvMessage(std::string & message) override;
        void startProtocol() override;


    private:
        int HandleError(int err);
        int HandleLogin();
        int HandlePORT();
        int HandleLIST();
        int HandleCWD();
        int HandleRETR();
        int HandleTYPE();

    private:
        int m_status;
        std::string m_recvMessage;
        std::string m_sendMessage;
        ConnectSocket* m_client;
        std::string m_pwd;
};
#endif
