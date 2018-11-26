#include "Socket.h"
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <glog/logging.h>

BaseSocket::BaseSocket(int socket)
    : m_socket(socket)
{
    LOG(INFO) << "BaseSocket(" << socket << ")" << std::endl;
    if(socket == -1){
        std::string errmsg = std::string("BaseSocket::") + __func__+": bad socket: "+ strerror(errno);
       LOG(INFO) << errmsg << std::endl;
        //throw std::runtime_error(errmsg);
    }
}

BaseSocket::~BaseSocket()
{
    LOG(INFO) << "~BaseSocket(" << m_socket << ")" << std::endl;
    if(m_socket == invalidSocket){
        LOG(INFO) << "~BaseSocket() invalidSocket" << std::endl;
        return;
    }

    try{
        close();
    } catch(...){
        ///loggging this
    }
}

void BaseSocket::close()
{
    LOG(INFO) << "BaseSocket::close()" << std::endl;
    if(m_socket == invalidSocket){
        std::string errmsg = std::string("BaseSocket::")+ __func__+": bad socket";
        throw std::runtime_error(errmsg);
    }

    int state = ::close(m_socket);
    if(state == invalidSocket){
        std::string errmsg = std::string("BaseSocket::")+ __func__+": " + strerror(errno);
        throw std::runtime_error(errmsg);

    }
    LOG(INFO) << "BaseSocket::close(" << m_socket << ") OK" << std::endl;
    m_socket = invalidSocket;
}

void BaseSocket::swap(BaseSocket& baseSocket) noexcept{
    LOG(INFO) << "BaseSocket::swap()" << std::endl;
    using std::swap;
    swap(m_socket, baseSocket.m_socket);
}

BaseSocket::BaseSocket(BaseSocket&& baseSocket) noexcept
    : m_socket(invalidSocket) {
    LOG(INFO) << "BaseSocket(BaseSocket&&)" << std::endl;
    baseSocket.swap(*this);
}

BaseSocket& BaseSocket::operator=(BaseSocket&& baseSocket) noexcept
{
    LOG(INFO) << "BaseSocket::= &&" << std::endl;
    baseSocket.swap(*this);
    return *this;
}

//SSLSocket::SSLSocket(int socket)
//            : BaseSocket(socket)
//            , m_ssl(NULL)
//{
//    LOG(INFO) << "SSLSocket(int socket):" << socket << std::endl;
//    //SSLCtx ctx;
//    //newSSL(ctx);
//}

SSLSocket::SSLSocket(int socket, SSLCtx* ctx)
            : BaseSocket(socket)
            , m_sslctx(ctx)
            , m_ssl(nullptr)
{
    LOG(INFO) << "SSLSocket(int socket):" << socket << std::endl;
    //if(ctx){
    //    m_sslctx = ctx;
    //    LOG(INFO) << "SSLSocket(int socket): m_sslctx " << m_sslctx->getCtx() << std::endl;
    //
    //}
    //newSSL(ctx);
}

int SSLSocket::newSSL()//SSLCtx ctx)
{
    LOG(INFO) << "newSSL(): ctx:" << m_sslctx->getCtx() << std::endl;
    if(m_sslctx->getCtx() == nullptr){
        std::string errmsg = std::string("SSLSocket::")+ __func__+": bad ctx";
        throw std::runtime_error(errmsg);
    }
    m_ssl = SSL_new(m_sslctx->getCtx());
    LOG(INFO) << "newSSL():" << m_ssl << std::endl;
    if( m_ssl == NULL )
    {
        std::string errmsg = std::string("SSLSocket:")+ __func__+": SSL_new: " + strerror(errno);
        throw std::runtime_error(errmsg);
    }
    LOG(INFO) << "SSL_set_fd :" << m_ssl << "," << getSocket() << std::endl;
    int retVal = SSL_set_fd(m_ssl, getSocket());
    if(retVal){
        LOG(INFO) << "SS:_set_fd OK : " << m_ssl << "," << getSocket() << std::endl;
    } else {
        LOG(INFO) << "SS:_set_fd Fail : " << m_ssl << "," << getSocket() << std::endl;
    }
    SSL_set_mode(m_ssl, SSL_MODE_AUTO_RETRY);
    SSL_set_mode(m_ssl, SSL_MODE_ENABLE_PARTIAL_WRITE);
    return 0;
}

SSLSocket::~SSLSocket()
{
    LOG(INFO) << "~SSLSocket()" << std::endl;
    closeSSL();
}

void SSLSocket::closeSSL()
{
    LOG(INFO) << "closeSSL()" << m_ssl << std::endl;
    if(!m_ssl){
        LOG(INFO) << "closeSSL() !m_ssl" << std::endl;
        return;
    }
    try{
        std::string ssl_state = SSL_state_string(m_ssl);

        int retVal = 0;
        int idx = 0;
        while(true){
            LOG(INFO) << "SSL_shutdown before:" << ssl_state << " , " << m_ssl <<"," << idx << std::endl;
            retVal = SSL_shutdown(m_ssl);
            if(retVal == 0){
                LOG(INFO) << "SSL_shutdown not yet " << idx << std::endl;
                idx++;
                continue;
            } 
            if( retVal == 1)
                LOG(INFO) << "SSL_shutdown OK" << std::endl;
            else
                LOG(INFO) << "SSL_shutdown Error" << std::endl;

            break;
        }
        LOG(INFO) << "SSL_free before(" << m_ssl << ")" << std::endl;
        SSL_free(m_ssl);
        m_ssl = NULL;
        LOG(INFO) << "SSL_free after" << std::endl;
    } catch(...){
        LOG(INFO) << "SSL_free(" << m_ssl << ")" << std::endl;
    }
}

void SSLSocket::swap(SSLSocket& sslSocket) noexcept{
    LOG(INFO) << "SSLSocket::swap()" << std::endl;
    using std::swap;
    swap(m_ssl, sslSocket.m_ssl);
    swap(m_sslctx, sslSocket.m_sslctx);
    swap(m_socket, sslSocket.m_socket);
}

SSLSocket::SSLSocket(SSLSocket&& sslSocket) noexcept
    : BaseSocket(invalidSocket)
    , m_ssl(nullptr) {
    LOG(INFO) << "SSLSocket(SSLSocket&&)" << std::endl;
    sslSocket.swap(*this);
}

SSLSocket& SSLSocket::operator=(SSLSocket&& sslSocket) noexcept
{
    LOG(INFO) << "SSLSocket::= &&" << std::endl;
    sslSocket.swap(*this);
    return *this;
}


IOSocket::IOSocket(SSLSocket&& sslSocket)
    : SSLSocket(std::move(sslSocket))
{
    LOG(INFO) << "IOSocket(SSLSocket&& socket)" << m_socket << std::endl;
}

int IOSocket::putMessage(char const* buffer, std::size_t size)
{
    std::size_t dataWritten = 0;
    while(dataWritten < size){
        int put;
        if(getSSL() != nullptr){

            ERR_clear_error();
            put = SSL_write(getSSL(), buffer + dataWritten, size - dataWritten);
            if (put <= 0){
                LOG(INFO) << "SSL_write: " << getSSL() << "," << put << std::endl;
                put=SSL_get_error(getSSL(),put);
                switch(put)
                {
                    case SSL_ERROR_ZERO_RETURN:
                        LOG(INFO) << "SSL_ERROR_ZERO_RETURN"; break;
                    case SSL_ERROR_WANT_READ:
                        LOG(INFO) << "SSL_ERROR_WANT_READ"; break;
                    case SSL_ERROR_WANT_WRITE:
                        LOG(INFO) << "SSL_ERROR_WANT_WRITE"; break;

                    case SSL_ERROR_WANT_CONNECT: 
                        LOG(INFO) << "SSL_ERROR_WANT_CONNECT"; break;
                    case SSL_ERROR_WANT_ACCEPT:
                        LOG(INFO) << "SSL_ERROR_ZERO_WANT_ACCEPT"; break;

                    case SSL_ERROR_WANT_X509_LOOKUP:
                        LOG(INFO) << "SSL_ERROR_WANT_X509_LOOKUP"; break;
                    case SSL_ERROR_SYSCALL:
                        LOG(INFO) << "SSL_ERROR_SYSCALL"; break;
                    case SSL_ERROR_SSL:
                        LOG(INFO) << "SSL_ERROR_SSL"; break;
                    default:
                        LOG(INFO) << "SSL_ERROR"; break;
                }
                LOG(INFO) << put <<  std::endl;
                //return -1;
                std::string errmsg = std::string("IOSocket::")+ __func__+": write error: "+ strerror(errno);
                throw std::runtime_error(errmsg);
            }
        } else { 
            put = write(getSocket(), buffer + dataWritten, size - dataWritten);
            if(put == -1){

                LOG(INFO) << "write " << getSocket() << " error(" << errno << ")" << "[" << strerror(errno) << "]" ;
                std::string errmsg = std::string("IOSocket::")+ __func__+": write error: "+ strerror(errno);
                throw std::runtime_error(errmsg);
            }

        }
        //LOG(INFO) << "WRITE OK:" << getSocket() << "," << getSSL() << "," << put << "," << dataWritten << std::endl;
        dataWritten += put;
    }
    return 0;
}

void IOSocket::putClose()
{
    closeSSL();
    if(::shutdown(getSocket(), SHUT_WR) != 0){
        std::string errmsg = std::string("IOSocket::")+ __func__+": shutdown: critical error: " + strerror(errno);
        throw std::runtime_error(errmsg);
    }
    return;
}

/*
ConnectSocket::ConnectSocket(std::string const& host, int port)
    : IOSocket(::socket(PF_INET, SOCK_STREAM, 0))
{
    struct sockaddr_in serverAddr{};
    serverAddr.sin_family       = AF_INET;
    serverAddr.sin_port         = htons(port);
    serverAddr.sin_addr.s_addr  = inet_addr(host.c_str());

    if(::connect(getSocket(), (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0 ){
        close();
        std::string errmsg = std::string("ConnectSocket::")+ __func__+": connect : " + strerror(errno);
        throw std::runtime_error(errmsg);
    }
}
*/

ConnectSocket::ConnectSocket(std::string const& host, int port, SSLCtx* ctx)
    : IOSocket(
            SSLSocket(::socket(PF_INET, SOCK_STREAM, 0), ctx))
{
    struct sockaddr_in serverAddr{};
    serverAddr.sin_family       = AF_INET;
    serverAddr.sin_port         = htons(port);
    serverAddr.sin_addr.s_addr  = inet_addr(host.c_str());

    if(::connect(getSocket(), (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0 ){
        close();
        std::string errmsg = std::string("ConnectSocket::")+ __func__+": connect : " + strerror(errno);
        throw std::runtime_error(errmsg);
    }
    LOG(INFO) << "connect:"  << getSSL() << std::endl;

    if(getSslCtx()==nullptr)
        return;

    try{
        newSSL();
    }catch(...){
        LOG(INFO) << "newSSL: error"  << getSSL() << std::endl;
    }
    //LOG(INFO) << "newSSL:" << getSSL() << std::endl;
    //SSL_set_fd(getSSL(), getSocket());
    //LOG(INFO) << "SSL_set_fd" << std::endl;
    if( SSL_connect(getSSL()) == -1){
        close();
        std::string errmsg = std::string("ConnectSocket::")+ __func__+": SSL connect : " + strerror(errno);
        throw std::runtime_error(errmsg);
    }
}


SSLCtx::SSLCtx(int isServer)
{
    LOG(INFO) << "SSLCtx()" << std::endl;

    OpenSSL_add_all_algorithms();		/* Load cryptos, et.al. */
    //OpenSSL_add_ssl_algorithms();
    SSL_load_error_strings();			/* Bring in and register error messages */
    SSL_library_init();
    SSL_METHOD *method;
    if(isServer)
        method = (SSL_METHOD*)SSLv23_server_method();		/* Create new client-method instance */
    else
        method = (SSL_METHOD*)SSLv23_client_method();		/* Create new client-method instance */
    m_ctx = SSL_CTX_new(method);			/* Create new context */
    if ( m_ctx == NULL )
    {
        std::string errmsg = std::string("SSLCtx::")+ __func__+": SSL_CTX_new: " + strerror(errno);
        throw std::runtime_error(errmsg);
    }
    LOG(INFO) << "m_ctx:" << m_ctx << std::endl;
}

SSLCtx::SSLCtx(const char* CertFile,const char* KeyFile)
{
    LoadFile(CertFile, KeyFile);
}

void SSLCtx::LoadFile(const char* CertFile,const char* KeyFile)
{
    LOG(INFO) << m_ctx << " LoadFile(" << CertFile << ", const char*)" << std::endl;
    if ( m_ctx == NULL )
    {
        std::string errmsg = std::string("SSLCtx::")+ __func__+": SSL_CTX_new: " + strerror(errno);
        throw std::runtime_error(errmsg);
    }
    //SSL_CTX_set_ecdh_auto(m_ctx, 1);
    /* set the local certificate from CertFile */
    if ( SSL_CTX_use_certificate_file(m_ctx, CertFile, SSL_FILETYPE_PEM) <= 0 )
    {
        LOG(INFO) << "SSL_CTX_use_certificate_file" << std::endl;
        std::string errmsg = std::string("SSLSocket::")+ __func__+": SSL_CTX_user_certificate_file: " + strerror(errno);
        throw std::runtime_error(errmsg);
    }
    LOG(INFO) << "SSL_CTX_use_certificate_file" << std::endl;
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file(m_ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 )
    {
        std::string errmsg = std::string("SSLSocket::")+ __func__+": SSL_CTX_user_PrivateKey_file: " + strerror(errno);
        throw std::runtime_error(errmsg);
    }
    LOG(INFO) << "SSL_CTX_use_Privatekey_file" << std::endl;
    /* verify private key */
    if ( !SSL_CTX_check_private_key(m_ctx) )
    {
        std::string errmsg = std::string("SSLSocket::")+ __func__+": SSL_CTX_check_private_key: " + strerror(errno);
        throw std::runtime_error(errmsg);
    }
    LOG(INFO) << "SSL_CTX_check_private_key" << std::endl;
}
SSLCtx::~SSLCtx()
{
    LOG(INFO) << "~SSLCtx()" << std::endl;
    if(m_ctx){
        LOG(INFO) << "SSL_CTX_free" << std::endl;
        SSL_CTX_free(m_ctx);
        m_ctx = NULL;
    }
}

SSL_CTX* SSLCtx::getCtx()
{
    return m_ctx;
}

ServerSocket::ServerSocket(int port, SSLCtx* ctx)
//    : SSLSocket(::socket(PF_INET, SOCK_STREAM, 0 ), nullptr)
    : SSLSocket(::socket(PF_INET, SOCK_STREAM, 0 ), ctx)
{
    LOG(INFO) << "ServerSocketSS()" << std::endl;
    struct sockaddr_in serverAddr;
    bzero((char*)&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family       = AF_INET;
    serverAddr.sin_port         = htons(port);
    serverAddr.sin_addr.s_addr  = INADDR_ANY;

    LOG(INFO) << "ServerSocketSS() reuse" << std::endl;
    int bf = 1;
    setsockopt(getSocket(), SOL_SOCKET, SO_REUSEADDR, &bf, sizeof(bf));

    LOG(INFO) << "ServerSocket:: bind" << std::endl;
    if(::bind(getSocket(), (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0 ){
        close();
        std::string errmsg = std::string("ServerSocket::")+ __func__+": bind: " + strerror(errno);
        throw std::runtime_error(errmsg);
    }

    LOG(INFO) << "ServerSocket:: listen" << std::endl;
    if(::listen(getSocket(), maxConnectionBacklog) != 0 ){
        close();
        std::string errmsg = std::string("ServerSocket::")+ __func__+": listen: " + strerror(errno);
        throw std::runtime_error(errmsg);
    }

    LOG(INFO) << "ServerSocket:: m_ctx:" << ctx << std::endl;
    LOG(INFO) << "ServerSocket:: m_ctx:" << getSslCtx() << std::endl;
    if(getSslCtx()==nullptr)
        return;


    try{
        //m_sslctx = getSslCtx();
        //m_sslctx = new SSLCtx(1);
        //LOG(INFO) << "ServerSocket:: m_sslctx-ctx:" << m_sslctx->getCtx() << std::endl;
        //m_sslctx->LoadFile("example.crt", "example.key");
        getSslCtx()->LoadFile("example.crt", "example.key");
        //m_sslctx->LoadFile("/home/ho80/devel/Project/automake_test/src/server.cakey.pem", "/home/ho80/devel/Project/automake_test/src/root.crt");
        LOG(INFO) << "LoadFile" << std::endl;
    } catch(std::runtime_error& e) {
        closeSSL();
        LOG(INFO) << e.what() << std::endl;
        throw;
    }
}

IOSocket ServerSocket::accept()
{
    LOG(INFO) << "ServerSocket::accept()" << std::endl;
    if(getSocket() == invalidSocket){
        std::string errmsg = std::string("ServerSocket::")+ __func__+": accept called on a bad socket object (this object was moved)";
        throw std::runtime_error(errmsg);
    }

    struct sockaddr_storage serverStorage;
    socklen_t               addr_size = sizeof(serverStorage);
    int newSocket = ::accept(getSocket(), (struct sockaddr*)&serverStorage, &addr_size);
    if(newSocket == -1 ){
        LOG(INFO) << "accept error" << std::endl;
        std::string errmsg = std::string("ServerSocket::")+ __func__+": accept :" + strerror(errno);
        throw std::runtime_error(errmsg);
    }
    LOG(INFO) << "accept OK : " << newSocket << std::endl;
    SSLSocket newssl(newSocket, getSslCtx());

    if(getSslCtx()){
        newssl.newSSL();
        if(SSL_accept(newssl.getSSL())<=0){
            LOG(INFO) << "SSL_accpet Error: " << newSocket << std::endl;
            std::string errmsg = std::string("ServerSocket::")+ __func__+": SSL_accept :" + strerror(errno);
            throw std::runtime_error(errmsg);

        }
        LOG(INFO) << "SSL_accpet OK:" << newSocket << std::endl;
    }

    return IOSocket(std::move(newssl));
}
