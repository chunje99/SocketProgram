#include "Socket.h"
#include "ProtocolSimple.h"
#include "ProtocolHttp.h"
#include <cstdlib>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]) 
{
    if(argc != 4){
        std::cerr << "Usage: client <host> <port> <docfile>" << std::endl;
        std::exit(1);
    }

    std::string host = argv[1];
    int port = atoi(argv[2]);
    std::string docName = argv[3];
    struct hostent *hp = gethostbyname(host.c_str());
    if(hp==NULL){
        std::cout << "gethostbyname fail:" << host << std::endl;
    } else {
        for (int ndx = 0; hp->h_addr_list[ndx] != NULL; ndx++){
            host = inet_ntoa( *(struct in_addr*)hp->h_addr_list[ndx]);
            std::cout << "gethostbyname " << host << std::endl;
        }
    }
    try{
        //ConnectSocket connect(host, port);
        ConnectSocket connect(host, port);
        //ProtocolSimple simpleConnect(connect);
        ProtocolHttp httpConnect(connect);
        httpConnect.setRequestHost(argv[1]);

        //httpConnect.sendRequest();
        httpConnect.sendRequestFd(docName.c_str());

        std::string message;
        httpConnect.recvMessage(message);
        std::cout << "size:" << message.size() << "," << message << std::endl;
        sleep(10);
    } catch(std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }catch(...){
        std::cout << "catch" << std::endl;
    }
    return 0;
}
