#include "iostream"
#include "Elevator.h"
#include "Person.h"
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <signal.h>

using namespace std;
using chronoClock=std::chrono::system_clock;

void daemon_init(){
    int pid;

    // fork 생성
    pid = fork();
    printf("pid = [%d] \n", pid);

    // fork 에러 발생 시 로그 남긴 후 종료
    if(pid < 0){
        printf("fork Error... : return is [%d] \n", pid );
        perror("fork error : ");
        exit(0);
        // 부모 프로세스 로그 남긴 후 종료
    }else if (pid > 0){
        printf("child process : [%d] - parent process : [%d] \n", pid, getpid());
        exit(0);
        // 정상 작동시 로그
    }else if(pid == 0){
        printf("process : [%d]\n", getpid());
    }

    // 터미널 종료 시 signal의 영향을 받지 않도록 처리
    signal(SIGHUP, SIG_IGN);
    //close(0);
    //close(1);
    //close(2);

    // 실행위치를 Root로 변경
    int v = chdir("/");

    // 새로운 세션 부여
    setsid();
}

int main(){
    cout << "HELLO" << endl;
    cout << "=============" << endl;
    vector<string> Vec;
    vector<string> Vec2;
    mutex mainMutex;
    thread t1([&mainMutex](vector<string>& vec){
            for(int i = 0; i < 100000000 ; i++) {
                //lock_guard<mutex> lock(mainMutex);
                vec.emplace_back("aaaa");
                //vec.push_back("aaaa");
            }
            }, ref(Vec));
    thread t2([&mainMutex](vector<string>& vec){
            for(int i = 0; i < 100000000 ; i++) {
                //lock_guard<mutex> lock(mainMutex);
                vec.emplace_back("dddd");
                //vec.push_back("dddd");
            }
            }, ref(Vec2));
    t1.join();
    t2.join();

    chronoClock::time_point start = chronoClock::now();
    vector<string> Vec3 = Vec;
    cout << Vec.size() << endl;

    chronoClock::time_point end = chronoClock::now();
    std::chrono::duration<double> sec = end - start;
    std::cout << "Test() 함수를 수행하는 걸린 시간(초) : " << sec.count() << " seconds" << std::endl;

    cout << "PID" << getpid() << "calling daemon()" << endl;
    //daemon_init();
    if( daemon(0,1) != 0) {
        cerr << "deamon() fail" << endl;
        exit(-1);
    }
    cerr << "daemon() OK: " << endl;
    fprintf( stderr, "deamon() OK\n" );
    printf( "deamon() OK\n" );
    sleep(10);
    return 0;
}
