#ifndef PROTOCOL_HTTP_H
#define PROTOCOL_HTTP_H

#include "Protocol.h"
#include <vector>
#include "fstream"

class ProtocolHttp: public Protocol
{
    public:
        using Protocol::Protocol;
        ProtocolHttp(IOSocket& socket);
        ~ProtocolHttp();
        void sendMessage(std::string const& message, int timeout=-1) override;
        void recvMessage(std::string & message, int timeout=-1) override;
        void startProtocol() override;
		void setRequestMethod(std::string method);
		void setRequestPath(std::string path);
        void setRequestVersion(std::string version);
        void setRequestHost(std::string host);
        void setRequestHeader(std::vector<std::string> header);

        int sendRequest();
        int sendRequestFd(const char* filename);
        int recvRequest(std::string& message);

        int sendResponse();

    private:
        int _ReadLine(const char* buf, size_t size);
        int _ParseRequestHeader(const char* buf, size_t size);
        int _SendBody();
        int _GetBodySize(size_t& bodySize);

        int _ResponseError(int errorcode);


    private:
        int m_status;
        std::string m_requestMethod;
        std::string m_requestPath;
        std::string m_requestVersion;
        std::string m_requestHost;
        std::vector<std::string> m_requestHeader;
        int m_responseCode;
        size_t m_rangeStart;
        size_t m_rangeEnd;
        bool m_isRange;
        std::ifstream m_file;
};
#endif
