#include <stdexcept>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "iostream"
#include <exception>
#include <string>
#include <glog/logging.h>

template<typename F>
std::size_t IOSocket::getMessage(char* buffer, std::size_t size, int timeout, F scanForEnd)
{
    if (getSocket() == 0)
    {
        std::string errmsg = std::string("IOSocket::") + __func__ + ": accept called on a bad socket object (this object was moved)";
        throw std::runtime_error(errmsg);
    }

    std::size_t     dataRead  = 0;
    struct timeval tv;
    fd_set readfds;
    int state;
    while(dataRead < size)
    {
        // The inner loop handles interactions with the socket.
        int get;
        if(getSSL() != nullptr){

            get = SSL_read(getSSL(), buffer + dataRead, size - dataRead);
            if (get <= 0)
            {
                std::cout << "getMessage error :" << get << " read: " << dataRead << std::endl;
                get=SSL_get_error(getSSL(),get);
                switch(get)
                {
                    case SSL_ERROR_ZERO_RETURN:
                        std::cout << "SSL_ERROR_ZERO_RETURN"; break;
                    case SSL_ERROR_WANT_READ:
                        std::cout << "SSL_ERROR_WANT_READ"; break;
                    case SSL_ERROR_WANT_WRITE:
                        std::cout << "SSL_ERROR_WANT_WRITE"; break;

                    case SSL_ERROR_WANT_CONNECT: 
                        std::cout << "SSL_ERROR_WANT_CONNECT"; break;
                    case SSL_ERROR_WANT_ACCEPT:
                        std::cout << "SSL_ERROR_ZERO_WANT_ACCEPT"; break;

                    case SSL_ERROR_WANT_X509_LOOKUP:
                        std::cout << "SSL_ERROR_WANT_X509_LOOKUP"; break;
                    case SSL_ERROR_SYSCALL:
                        std::cout << "SSL_ERROR_SYSCALL"; break;
                    case SSL_ERROR_SSL:
                        std::cout << "SSL_ERROR_SSL"; break;
                    default:
                        std::cout << "SSL_ERROR"; break;
                        break;
                }
                std::cout << get <<  std::endl;
                std::string errmsg = std::string("IOSocket::") + __func__ + ": SSL_get()";
                throw std::runtime_error(errmsg);
                break;
            }
        }else{
            if(timeout==-1){
                state = 1;
            }else{
                // select time out 설저을 위한 timeval 구조체   
                FD_ZERO(&readfds);
                FD_SET(getSocket(), &readfds);
                // 약 5초간 기다린다. 
                tv.tv_sec = timeout;
                tv.tv_usec = 0;

                // 입력이 있는지 기다린다. 
                state = select(getSocket()+1, &readfds,
                        (fd_set *)0, (fd_set *)0, &tv);
            }
            std::string errmsg;
            switch(state)
            {
                case -1:
                    std::cout << "read select error:" << getSocket() <<  std::endl;
                    errmsg = std::string("IOSocket::") + __func__ + ": read select error";
                    throw std::runtime_error(errmsg);
                    break;
                    // 만약 5초안에 아무런 입력이 없었다면 
                    // Time out 발생상황이다. 
                case 0:

                        std::cout << "read timeout:" << getSocket() <<  std::endl;
                        errmsg = std::string("IOSocket::") + __func__ + ": read timeout";
                        throw std::runtime_error(errmsg);
                        break;
                        // 5초안에 입력이 들어왔을경우 처리한다. 
                default:
                       get = read(getSocket(), buffer + dataRead, size - dataRead);
                       if(get==-1){
                           std::cout << "read error: " <<  errno << "," << strerror(errno) <<  std::endl;
                           errmsg = std::string("IOSocket::") + __func__ + ": getSocket()";
                           throw std::runtime_error(errmsg);
                           break;

                       }
                       break;
            }
        }
        if (get == 0)
        {
            std::cout << "dataRead: get == 0" << std::endl;
            break;
        }
        dataRead += get;
        if (scanForEnd(get, buffer+dataRead-get))
        {
            break;
        }
    }

    return dataRead;
}
