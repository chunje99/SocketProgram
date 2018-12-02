#include "ProtocolFtp.h"
#include "ProtocolSimple.h"
#include "Socket.h"
#include "functional"
#include "string.h"
#include "ctime"
#include "stdio.h"
#include <glog/logging.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>

ProtocolFtp::ProtocolFtp(IOSocket& socket)
    : Protocol(socket)
    ,m_status(0)
      ,m_client(nullptr)
      ,m_pwd("/home/ho80/")
{
}
ProtocolFtp::~ProtocolFtp(){
    LOG(INFO) << "~ProtocolFtp()" << std::endl;
}

void ProtocolFtp::sendMessage(std::string const& message, int timeout)
{
    m_socket.putMessage(message.c_str(), message.size());
}

void ProtocolFtp::recvMessage(std::string & message, int timeout)
{
	message.clear();
	message.reserve(8092);
	char buffer[8093];
    std::size_t const dataMax = 8092;

	std::size_t got = m_socket.getMessage(buffer, dataMax , -1,  [](std::size_t s, const char* buf) -> int { return true;});
    if(got == 0){
        std::string errmsg = std::string("ProtocolFtp::") + __func__+": got == 0";
        throw std::runtime_error(errmsg);
    }
	LOG(INFO).write(buffer, got);
    message.assign(buffer,got);
	//   if(got == 0)
	//      break;
	//}
}

void ProtocolFtp::startProtocol()
{
	int retVal;
	m_sendMessage = "220 (ho80 server)\r\n";
	sendMessage(m_sendMessage);
	LOG(INFO) << "[server]::send " << m_sendMessage << std::endl;
    while(HandleLogin() != 0 ){
    }
	//if(retVal != 0)
	//	return;

	while(1){
		recvMessage(m_recvMessage);
		LOG(INFO) << "[server]::m_recvMessage " << m_recvMessage << std::endl;
		if(m_recvMessage.find("ABOR") != std::string::npos){// ABOR		현재의 파일 전송 중단.
            HandleError(500);
		} else if(m_recvMessage.find("ACCT") != std::string::npos){// ACCT		계정 정보.
            HandleError(500);
		} else if(m_recvMessage.find("ADAT") != std::string::npos){//	RFC 2228	인증/보안 데이터
            HandleError(500);
		} else if(m_recvMessage.find("ALLO") != std::string::npos){//		파일을 받기 위해 충분한 디스크 공간 할당.
            HandleError(500);
		} else if(m_recvMessage.find("APPE") != std::string::npos){//	이어서 추가.
            HandleError(500);
		} else if(m_recvMessage.find("AUTH") != std::string::npos){//	RFC 2228	인증/보안 구조
            HandleError(500);
		} else if(m_recvMessage.find("CCC") != std::string::npos){//	RFC 2228	명령 채널 지우기
            HandleError(500);
		} else if(m_recvMessage.find("CDUP") != std::string::npos){//		부모 디렉터리로 변경.
            HandleError(500);
		} else if(m_recvMessage.find("CONF") != std::string::npos){//	RFC 2228	기밀 보호 명령
            HandleError(500);
		} else if(m_recvMessage.find("CWD") != std::string::npos){//		작업 디렉터리 변경
            HandleCWD();
		} else if(m_recvMessage.find("DELE") != std::string::npos){//		파일 삭제
            HandleError(500);
		} else if(m_recvMessage.find("ENC") != std::string::npos){//	RFC 2228	개인 정보 보호 채널
            HandleError(500);
		} else if(m_recvMessage.find("EPRT") != std::string::npos){//	RFC 2428	서버 접속에 필요한 확장 주소 및 포트 지정.
            HandleError(500);
		} else if(m_recvMessage.find("EPSV") != std::string::npos){//	RFC 2428	확장 수동 모드 들어가기.
            HandleError(500);
		} else if(m_recvMessage.find("FEAT") != std::string::npos){//	RFC 2389	서버가 추가한 기능 목록 보기
            HandleError(500);
		} else if(m_recvMessage.find("LANG") != std::string::npos){//	RFC 2640	언어 탐색
            HandleError(500);
		} else if(m_recvMessage.find("LIST") != std::string::npos){//		지정한 경우 파일이나 디렉터리 정보를 반환. 지정하지 않은 경우 현재 작업 디렉터리 정보 반환.
            HandleLIST();
		} else if(m_recvMessage.find("LPRT") != std::string::npos){//	RFC 1639	서버 접속에 필요한 긴 주소 및 목록 지정.
            HandleError(500);
		} else if(m_recvMessage.find("LPSV") != std::string::npos){//	RFC 1639	긴 수동 모드 들어가기
            HandleError(500);
		} else if(m_recvMessage.find("MDTM") != std::string::npos){//	RFC 3659	지정한 파일의 마지막으로 수정한 시간 반환
            HandleError(500);
		} else if(m_recvMessage.find("MIC") != std::string::npos){//	RFC 2228	무결성 보호 명령
            HandleError(500);
		} else if(m_recvMessage.find("MKD") != std::string::npos){//		디렉터리 만들기
            HandleMKD();
		} else if(m_recvMessage.find("MLSD") != std::string::npos){//	RFC 3659	디렉터리의 이름이 지정되면 디렉터리의 내용을 보여줌
            HandleError(500);
		} else if(m_recvMessage.find("MLST") != std::string::npos){//	RFC 3659	명령 줄에 입력한 데이터만 제공.
            HandleError(500);
		} else if(m_recvMessage.find("MODE") != std::string::npos){//		전송 모드 설정 (스트림, 블록, 압축)
            HandleError(500);
		} else if(m_recvMessage.find("NLST") != std::string::npos){//		지정한 디렉터리의 파일 이름 목록 반환.
            HandleError(500);
		} else if(m_recvMessage.find("NOOP") != std::string::npos){//		동작 안 함 (더미 패킷: 대개 회선이 살아있는지를 살피기 위해 쓰임)
            HandleError(500);
		} else if(m_recvMessage.find("OPTS") != std::string::npos){//	RFC 2389	기능 옵션 선택.
            HandleError(500);
		} else if(m_recvMessage.find("PASS") != std::string::npos){//		암호.
            HandleError(500);
		} else if(m_recvMessage.find("PASV") != std::string::npos){//		수동 모드 들어가기.
            HandlePASV();
		} else if(m_recvMessage.find("PBSZ") != std::string::npos){//	RFC 2228	보호 버퍼 크기
            HandleError(500);
		} else if(m_recvMessage.find("PORT") != std::string::npos){//		서버 접속에 필요한 주소 및 포트 지정.
            HandlePORT();
		} else if(m_recvMessage.find("PROT") != std::string::npos){//	RFC 2228	데이터 채널 보호 수준.
            HandleError(500);
		} else if(m_recvMessage.find("PWD") != std::string::npos){//	작업 디렉터리 인쇄. 호스트 컴퓨터의 현재 디렉터리 반환.
            HandlePWD();
		} else if(m_recvMessage.find("QUIT") != std::string::npos){//		연결 끊기.
            HandleError(500);
		} else if(m_recvMessage.find("REIN") != std::string::npos){//		연결 다시 초기화.
            HandleError(500);
		} else if(m_recvMessage.find("REST") != std::string::npos){//		지정한 지점에서 전송 다시 시작.
            HandleError(500);
		} else if(m_recvMessage.find("RETR") != std::string::npos){//		파일 복사본 전송
            HandleRETR();
		} else if(m_recvMessage.find("RMD") != std::string::npos){//		디렉터리 제거
            HandleError(500);
		} else if(m_recvMessage.find("RNFR") != std::string::npos){//		이름 변경 원본 이름
            HandleError(500);
		} else if(m_recvMessage.find("RNTO") != std::string::npos){//		이름 변경 대상 이름
            HandleError(500);
		} else if(m_recvMessage.find("SITE") != std::string::npos){//		지정한 명령어를 원격 서버로 송신.
            HandleError(500);
		} else if(m_recvMessage.find("SIZE") != std::string::npos){//	RFC 3659	파일 크기 반환
            HandleSIZE();
		} else if(m_recvMessage.find("SMNT") != std::string::npos){//		파일 구조 마운트.
            HandleError(500);
		} else if(m_recvMessage.find("STAT") != std::string::npos){//		현재 상태 반환.
            HandleError(500);
		} else if(m_recvMessage.find("STOR") != std::string::npos){//		데이터 입력 및 서버 쪽 파일로 저장.
            HandleSTOR();
		} else if(m_recvMessage.find("STOU") != std::string::npos){//		파일을 저만의 방식으로 저장.
            HandleError(500);
		} else if(m_recvMessage.find("STRU") != std::string::npos){//		전송 구조 설정.
            HandleError(500);
		} else if(m_recvMessage.find("SYST") != std::string::npos){//		시스템 유형 반환.
            m_sendMessage = "215 UNIX Type: L8\n";
            sendMessage(m_sendMessage);
            LOG(INFO) << "[server]::m_recvMessage " << m_sendMessage << std::endl;
		} else if(m_recvMessage.find("TYPE") != std::string::npos){//		전송 모드 설정 (ASCII/바이너리).
            HandleTYPE();
		} else if(m_recvMessage.find("USER") != std::string::npos){//		인증 사용자 이름.
            //HandleError(500);
            HandleLogin();
		} else {
            HandleError(500);
		}
	}

}

int ProtocolFtp::HandleError(int err)
{
    switch(err){
        case 500:
            m_sendMessage = "500 Syntax error, command unrecognized.\n";
            break;
        case 530:
            m_sendMessage = "530 Please Login again.\n";
            break;
        default:
            m_sendMessage = "500 Syntax error, command unrecognized.\n";
            break;
    }
    sendMessage(m_sendMessage);
    LOG(INFO) << "[server]::message " << m_sendMessage << std::endl;
    return 0;
}

int ProtocolFtp::HandleLogin()
{
    try{
        recvMessage(m_recvMessage, 5);
        LOG(INFO) << "[server]::message " << m_recvMessage << std::endl;
    } catch(std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        m_sendMessage = "530 Login incorrect.\r\n";
        sendMessage(m_sendMessage);
	LOG(INFO) << "[server]::send " << m_sendMessage << std::endl;
        return -1;
    }catch(...){
        std::cout << "catch" << std::endl;
        return -1;
    }
    if(m_recvMessage.find("USER") == std::string::npos){
        HandleError(530);
        return -1;
    }
	m_sendMessage = "331 Please specify the password.\n";
	sendMessage(m_sendMessage);
	recvMessage(m_recvMessage, 5);
	LOG(INFO) << "[server]::message " << m_recvMessage << std::endl;
	m_sendMessage = "230 Login successful.\n";
	sendMessage(m_sendMessage);

	return 0;
}

int ProtocolFtp::HandlePORT()
{
    int i1,i2,i3,i4,p1,p2;
    int c = sscanf(m_recvMessage.data(), "PORT %d,%d,%d,%d,%d,%d",
            &i1,&i2,&i3,&i4,&p1,&p2);
    if(c != 6){
        return HandleError(500);
    }

    int port = p1*256 + p2;
    std::string host = std::to_string(i1) + "." + std::to_string(i2) + "." + std::to_string(i3) + "." + std::to_string(i4); 
    LOG(INFO) << "HandlePORT:" << host << ":" << port;

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
        LOG(INFO) << "Connection Before";
        if(m_client){
            delete(m_client);
            m_client = nullptr;
        }
        m_client = new ConnectSocket(host, port, nullptr);
        LOG(INFO) << "Connection after ";
        //m_client = new ProtocolSimple(connect);
        //ProtocolSimple ps(connect);
        //LOG(INFO) << "Connection " << connect.getSocket();
    } catch(std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }catch(...){
        std::cout << "catch" << std::endl;
    }

    m_sendMessage = "200 PORT command successful. Consider using PASV.\n";

    sendMessage(m_sendMessage);
    LOG(INFO) << "PORT OK";
    return 0;
}

int ProtocolFtp::HandleLIST()
{
    m_sendMessage = "150 Here comes the directory listing.\n";
    sendMessage(m_sendMessage);
    try{
        ProtocolSimple ps(*m_client);
        LOG(INFO) << "opendir : " <<  m_pwd;
        DIR* dirp = opendir(m_pwd.c_str());
        if(dirp==NULL)
            return HandleError(500);

        struct dirent * dp;
        struct stat sb;
        std::string fullPath;
        while ((dp = readdir(dirp)) != NULL) {
            char auth[11] = {'-'};
            std::stringstream ss;
            auth[10] = 0;
            fullPath = m_pwd + "/" + dp->d_name;
            if (stat(fullPath.c_str(), &sb) == -1) {
                LOG(INFO) << fullPath << " open error";
                continue;
            }
            switch (sb.st_mode & S_IFMT) {
                case S_IFBLK:  LOG(INFO) << "block device";break;
                case S_IFCHR:  LOG(INFO) << "character device";break;
                case S_IFDIR:  LOG(INFO) << "directory"; auth[0] = 'd'; ss << "d";break;
                case S_IFIFO:  LOG(INFO) << "FIFO/pipe";break;
                case S_IFLNK:  LOG(INFO) << "symlink";break;
                case S_IFREG:  LOG(INFO) << "regular file";auth[0]='-'; ss << "-";break;
                case S_IFSOCK: LOG(INFO) << "socket";break;
                default:       LOG(INFO) << "unknown?";auth[0] = '-'; ss << "-";break;
            }
            std::string timeStr(ctime(&sb.st_ctime));
            std::string sizeStr(std::to_string(sb.st_size));
            for(int i = sizeStr.size() ; i < 8 ;i++)
                sizeStr.push_back(' ');
            ss << "rwxr-xr-x    " << sb.st_nlink << " " << sb.st_uid << "     " << sb.st_uid << "     " << sizeStr << " Dec 24  2017 " << dp->d_name;
            LOG(INFO) << ss.str();
            m_sendMessage = ss.str();
            /*
            if(dp->d_type == DT_DIR){
                m_sendMessage = "[";
                m_sendMessage += dp->d_name;
                m_sendMessage += "]";
            } else
                m_sendMessage = dp->d_name;
            */
            m_sendMessage += "\r\n";
            ps.sendMessage(m_sendMessage);
        }
        closedir(dirp);
        delete m_client;
        m_client = nullptr;
    } catch(std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }catch(...){
        std::cout << "catch" << std::endl;
    }
    m_sendMessage = "226 Directory send OK.\n";
    sendMessage(m_sendMessage);
    LOG(INFO) << "Connection OK";
    return 0;
}

int ProtocolFtp::HandleRETR()
{
    m_sendMessage = "150 file open\n";
    sendMessage(m_sendMessage);

    char name[512];
    int c = sscanf(m_recvMessage.data(), "RETR %s", name);
    if(c != 1 ){
        return HandleError(500);
    }

    std::string filename = m_pwd + "/" + name;
    std::ifstream m_file;
    m_file.open( filename.c_str(),std::ios::in|std::ios::binary);
    if (!m_file.is_open()){
        LOG(INFO) << "file open error:" << filename;
        return HandleError(500);
    }

    //m_file.seekg(m_rangeStart);
    int bufSize = 4096*100;
    char buf[bufSize+4];
    int offset = 0;
    size_t totalSize = 0;
    char t;
    int retVal = 0;
    try{
        ProtocolSimple ps(*m_client);
        m_sendMessage.clear();
        while (m_file.get(t))
        {
            m_sendMessage.push_back(t);
            /*
            buf[offset++] = t;
            if(offset==bufSize){
                //retVal = m_socket.putMessage(buf, offset);
                if(retVal != 0 ){
                    LOG(INFO) << "putMessage Error" << std::endl;
                    return -1;
                }
                totalSize += offset;
                offset = 0;
                memset(buf, 0, bufSize);
            }
            */
        }
        ps.sendMessage(m_sendMessage);
        delete m_client;
        m_client = nullptr;
        LOG(INFO) << "putMessage :" << m_sendMessage;
        //if(offset != 0)
        //    m_socket.putMessage(buf, offset);

    } catch(std::runtime_error& e) {
        LOG(INFO) << e.what() << std::endl;
    } catch(...){
        LOG(INFO) << "putMessage Error" << std::endl;
    }

    m_sendMessage = "226 file send OK.\n";
    sendMessage(m_sendMessage);
    LOG(INFO) << "Connection OK";
    return 0;
}

int ProtocolFtp::HandleCWD()
{
    char path[512];
    int c = sscanf(m_recvMessage.data(), "CWD %s", path);
    if(c != 1){
        return HandleError(500);
    }
    if(path[0] == '/')
        m_pwd = path;
    else{
        m_pwd += "/";
        m_pwd += path;
    }
    //m_pwd += "/";
    LOG(INFO) << "CWD " << path << "," << m_pwd;
    m_sendMessage = "250 Directory successfully changed.\r\n";
    sendMessage(m_sendMessage);
    return 0;
}

int ProtocolFtp::HandleTYPE()
{
    m_sendMessage = "200 okay.\n";
    sendMessage(m_sendMessage);
    return 0;
}

int ProtocolFtp::HandlePWD()
{
    m_sendMessage = "257 \"";
    m_sendMessage += m_pwd;
    m_sendMessage += "\" is the current directory\r\n";
    sendMessage(m_sendMessage);
    return 0;
}

int ProtocolFtp::HandleSTOR()
{
    try{
        ProtocolSimple ps(*m_client);
        ps.recvMessage(m_recvMessage);
        LOG(INFO) << "recvMessage:" << m_recvMessage;
        delete m_client;
        m_client = nullptr;

    } catch(std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
    }catch(...){
        std::cout << "catch" << std::endl;
    }
    m_sendMessage = "150 Ok to send data.\n";
    sendMessage(m_sendMessage);
    m_sendMessage = "226 Transfer complete.\n";
    sendMessage(m_sendMessage);
    LOG(INFO) << "STOR OK";
    return 0;
}

int ProtocolFtp::HandleSIZE()
{
    return 0;
}

int ProtocolFtp::HandlePASV()
{
    m_sendMessage = "227 Entering Passive Mode.\n";
    sendMessage(m_sendMessage);
    LOG(INFO) << "PASV OK";
    return 0;
}

int ProtocolFtp::HandleMKD()
{
    char name[512];
    int c = sscanf(m_recvMessage.data(), "MKD %s", name);
    if(c != 1 ){
        return HandleError(500);
    }
    m_sendMessage = "257 Make directory ";
    m_sendMessage += name;
    m_sendMessage += " complete.\n";
    sendMessage(m_sendMessage);
    LOG(INFO) << m_sendMessage;
    return 0;
}
