#include "Socket.h"
#include "ProtocolSimple.h"
#include "ProtocolMuxer.h"
#include <iostream>
#include <vector>
#include <thread>
#include <unistd.h>
#include <glog/logging.h>
#include "signal.h"

void PrintUsage(){
    std::cout << "server port" << std::endl;
    exit(1);
}
 
int main(int argc, char* argv[])
{
    signal(SIGPIPE, SIG_IGN);

    if( argc != 2 )
        PrintUsage();

    int port = atoi(argv[1]);

    // google log library를 설정합니다.
    google::InitGoogleLogging(argv[0]);
    // FLAGS_logtostderr 0 파일 1 콘솔창
    FLAGS_logtostderr = 1;
    // 로그를 남기 위치
    FLAGS_log_dir = "D:\\private\\Agent\\proj_11";

    LOG(INFO) << "[server] main" << argc << std::endl;
    SSLCtx* ctx = nullptr;
    if(port==443)
        ctx = new SSLCtx(1);

    try{
        ServerSocket    server(port, ctx);
        LOG(INFO) << "[server] server" << std::endl;
        int             finished = 0;
        std::vector<std::thread> threadVec;
        try {
            while(!finished){
                IOSocket accept = server.accept();
                LOG(INFO) << "[server] IOSocket" << std::endl;
                std::thread th([](IOSocket accept){
                        try{
                        LOG(INFO) << "[server] " << accept.getSocket() << std::endl;
                        //ProtocolSimple acceptSimple(accept);
                        ProtocolMuxer acceptMuxer(accept);
                        LOG(INFO) << "[server]::ProtocolSimple" << std::endl;
                        acceptMuxer.startProtocol();

                        } catch(std::runtime_error& e) {
                        std::cerr << e.what() << std::endl;
                        } catch(...) {
                        std::cerr << "Unknown exception" << std::endl;
                        }
                        }, std::move(accept));
                th.detach();
                LOG(INFO) << "[server]::after thread " << accept.getSocket() << std::endl;
                //threadVec.push_back(std::move(th));
            }
        } catch(std::runtime_error& e) {
            std::cerr << e.what() << std::endl;
        } catch(...) {
            std::cerr << "Unknown exception..." << std::endl;
        }
    } catch(std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }catch(...){
        LOG(INFO) << "catch" << std::endl;
    }
    LOG(INFO) << "END SERVER" << std::endl;
    return 0;
}
