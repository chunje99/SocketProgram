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
        void sendMessage(std::string const& message, int timeout=-1) override;
        void recvMessage(std::string & message, int timeout=-1) override;
        void startProtocol() override;


    private:
        int HandleError(int err);
        int HandleLogin();
        int HandlePORT();
        int HandleLIST();
        int HandleCWD();
        int HandleRETR();
        int HandleTYPE();
        int HandlePWD();
        int HandleSTOR();
        int HandleSIZE();
        int HandlePASV();
        int HandleMKD();

    private:
        int m_status;
        std::string m_recvMessage;
        std::string m_sendMessage;
        ConnectSocket* m_client;
        std::string m_pwd;
};
#endif
