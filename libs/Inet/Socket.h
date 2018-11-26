#ifndef BASESOCKET_H
#define BASESOCKET_H
#include <string>
#include <memory>
#include <openssl/ssl.h>
#include <openssl/err.h>

class BaseSocket
{
    public:
        BaseSocket(int socket);
        BaseSocket(BaseSocket&& baseSocket) noexcept;
        BaseSocket& operator=(BaseSocket&& baseSocket) noexcept;
        void swap(BaseSocket& baseSocket) noexcept;
        BaseSocket(BaseSocket const&) = delete;
        BaseSocket& operator=(BaseSocket const&) = delete;

        int getSocket() const {return m_socket;}
        ~BaseSocket();
        void close();

        static constexpr int invalidSocket = -1;

    //private:
    protected:
        int m_socket;
};

class SSLCtx
{
    public:
        SSLCtx(int isServer);
        SSLCtx(const char* CertFile,const char* KeyFile);
        void LoadFile(const char* CertFile,const char* KeyFile);
        ~SSLCtx();
        SSL_CTX* getCtx();
    private:
        SSL_CTX* m_ctx;
};

class SSLSocket: public BaseSocket
{
    public:
        //SSLSocket(int socket);
        SSLSocket(int socket, SSLCtx* ctx);
        SSLSocket(SSLSocket&& sslSocket) noexcept;
        SSLSocket& operator=(SSLSocket&& sslSocket) noexcept;
        void swap(SSLSocket& sslSocket) noexcept;

        SSL* getSSL() const {return m_ssl;}
        ~SSLSocket();
        int newSSL();
        void closeSSL();
        SSLCtx* getSslCtx(){return m_sslctx;};

        static constexpr int invalidSSL = -1;
    private:
        SSL* m_ssl;
        SSLCtx* m_sslctx;
};

class IOSocket: public SSLSocket
{
    public:
        //IOSocketS(int socket);
        IOSocket(SSLSocket&& sslSocket);
        template<typename F>
        std::size_t getMessage(char* buffer, std::size_t size,
                F scanForEnd = [](std::size_t){return false;});
        int         putMessage(char const* buffer, std::size_t size);
        void        putClose();

};

class ConnectSocket: public IOSocket
{
    public:
        ConnectSocket(std::string const& host, int port, SSLCtx* ctx);
    private:
        //SSLCtx* m_sslctx;
};


class ServerSocket: public SSLSocket
{
    static constexpr int maxConnectionBacklog = 5;
    public:
        ServerSocket(int port, SSLCtx* ctx);
        IOSocket accept();
    private:
        //SSLCtx* m_sslctx;
};

#include "Socket.tpp"

#endif 
