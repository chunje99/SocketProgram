#include "ProtocolHttp.h"
#include "Socket.h"
#include "functional"
#include "string.h"
#include "ctime"
#include "stdio.h"
#include <glog/logging.h>

ProtocolHttp::ProtocolHttp(IOSocket& socket)
    : Protocol(socket)
    ,m_status(0)
    ,m_requestMethod("GET")
    ,m_requestPath("/")
    ,m_requestVersion("1.1")
    ,m_requestHost("127.0.0.1")
    ,m_requestHeader({
		"user-agent: Mozilla/5.0 AppleWebKit/537.36 Chrome/69.0.3497.100 Safari/537.36\r\n",
		"cookie: \r\n",
		"Connection: close\r\n"})
    ,m_responseCode(0)
    ,m_rangeStart(0)
    ,m_rangeEnd(0)
    ,m_isRange(false)
    ,m_file(nullptr)
{
}
ProtocolHttp::~ProtocolHttp(){
    LOG(INFO) << "~ProtocolHttp()" << std::endl;
    if(m_file.is_open()){
        m_file.close();
        LOG(INFO) << "~closeFile()" << std::endl;
    }
}

void ProtocolHttp::sendMessage(std::string const& message)
{
    m_socket.putMessage(message.c_str(), message.size());
}

void ProtocolHttp::startProtocol()
{
}

class StringSizer
{
    private:
        std::string&    stringData;
        std::size_t&    currentSize;
    public:
        StringSizer(std::string& stringData, std::size_t& currentSize)
            : stringData(stringData)
              , currentSize(currentSize)
        {
            stringData.resize(stringData.capacity());
        }
        ~StringSizer()
        {
            stringData.resize(currentSize);
        }
        void incrementSize(std::size_t amount)
        {
            currentSize += amount;
        }
};

void ProtocolHttp::recvMessage(std::string & message)
{
    std::size_t dataRead = 0;
    message.clear();
    message.reserve(8092);

    //while(true){
   //    StringSizer stringSizer(message, dataRead);
   //    std::size_t const dataMax = message.capacity() -1;
   //    char* buffer = &message[0];

   //    std::size_t got = m_socket.getMessage(buffer + dataRead, dataMax - dataRead, [](std::size_t s, const char* buf) -> int { return true;});
   //    LOG(INFO).write(buffer+dataRead, got);
   //    dataRead += got;
   //    if(got == 0)
   //        break;
   //    message.reserve(message.capacity()*1.5 + 10);
   //}
    while(true){
        message.clear();
        std::size_t const dataMax = message.capacity() -1;
        char* buffer = &message[0];

        std::size_t got = m_socket.getMessage(buffer, dataMax , [](std::size_t s, const char* buf) -> int { return true;});
        LOG(INFO).write(buffer, got);
        if(got == 0)
            break;
    }
}

void ProtocolHttp::setRequestMethod(std::string method){
    m_requestMethod = method;
}
void ProtocolHttp::setRequestPath(std::string path){
    m_requestPath = path;
}
void ProtocolHttp::setRequestVersion(std::string version){
    m_requestVersion= version;
}
void ProtocolHttp::setRequestHeader(std::vector<std::string> header){
    m_requestHeader = header;
}
void ProtocolHttp::setRequestHost(std::string host){
    m_requestHost = host;
}

int ProtocolHttp::sendRequest(){
    std::string message = m_requestMethod + " " + m_requestPath + " HTTP/" + m_requestVersion + "\r\n";
    m_socket.putMessage(message.c_str(), message.size());
    message = "Host: " + m_requestHost + "\r\n";
    m_socket.putMessage(message.c_str(), message.size());
    for( auto& msg : m_requestHeader){
        m_socket.putMessage(msg.c_str(), msg.size());
    }
    message = "\r\n";
    m_socket.putMessage(message.c_str(), message.size());
    //m_socket.putClose();
    return 0;
}

int ProtocolHttp::sendRequestFd(const char* filename){
    LOG(INFO) << "ProtocolHttp::sendRequestFD()" << std::endl;
    std::ifstream file( filename,std::ios::in);
    if (!file.is_open()){
        LOG(INFO) << "file Open Error:" << filename << std::endl;
        return -1;
    } else
        LOG(INFO) << "file Open OK:" << filename << std::endl;

    std::string msgStr;
    std::string sendStr;
    msgStr.reserve(2048);
    char* msgBuf = &msgStr[0];
    file.seekg(0);
    while(!file.eof()){
        file.getline(msgBuf, 2048);
        sendStr = msgBuf;
        sendStr += "\r\n";
        LOG(INFO) << "MSG:" << sendStr << std::endl;;
        m_socket.putMessage(sendStr.c_str(), sendStr.size());
    }
    return 0;
}
int ProtocolHttp::recvRequest(std::string& message){
    std::size_t dataRead = 0;
    message.clear();
    message.reserve(2048);
    StringSizer stringSizer(message, dataRead);
    std::size_t const dataMax = message.capacity() -1;
    char* buffer = &message[0];

    std::size_t got = m_socket.getMessage(buffer + dataRead, dataMax - dataRead, [](std::size_t s, const char* buf) {
                LOG(INFO).write(buf,s);
                if(s==2 && strcmp(buf, "\r\n") == 0 )
                    return true;
                else if(s>=4){
                    std::string str = buf;
                    if( str.find("\r\n\r\n") != std::string::npos){
                        return true;
                    } else{
                        return false;
                    }
                }
                else {
                    LOG(INFO) << s << " scanEnd:" << buf << std::endl;
                    return false;
                }
            }
            );
    //LOG(INFO).write(buffer+dataRead, got);
    dataRead += got;
    //message.reserve(message.capacity()*1.5 + 10);
    return _ParseRequestHeader(buffer, dataRead);
}

int ProtocolHttp::_GetBodySize(size_t& bodySize){
    ///get file0
    std::string root = "/home/ho80/devel/Project/automake_test/src/docs";
    size_t uriPos = m_requestPath.find("?");
    std::string filename;
    if(uriPos == std::string::npos)
        filename = m_requestPath;
    else
        filename = m_requestPath.substr(uriPos-1);
    if(filename == "/")
        filename = "/index.html";
    filename = root + filename;

    //Range: bytes=2334654464-
    for(auto option : m_requestHeader){
        if(option.find("Range: bytes=") != std::string::npos){
            LOG(INFO) << option << std::endl;
            sscanf(option.c_str()+13, "%d-%d", &m_rangeStart, &m_rangeEnd);
            m_isRange = true;
        }
    }
    if (!m_file.is_open()){
        m_file.open( filename.c_str(),std::ios::in|std::ios::binary);
        if (!m_file.is_open())
            return 404;
    }

    //Content-Length: 1810025557
    //Content-Range: bytes 2334654464-4144680020/4144680021
    m_file.seekg (0, m_file.end);
    std::ifstream::streampos filesize = m_file.tellg();
    if(filesize < 0) filesize = 0;
    bodySize = filesize;
    LOG(INFO) << "FILESIZE:" << filesize << std::endl;
    //file.close();
    return 200;
}

int ProtocolHttp::_SendBody() {
    ///get file0
    std::string root = "/home/ho80/devel/Project/automake_test/src/docs";
    size_t uriPos = m_requestPath.find("?");
    std::string filename;
    if(uriPos == std::string::npos)
        filename = m_requestPath;
    else
        filename = m_requestPath.substr(uriPos-1);
    if(filename == "/")
        filename = "/index.html";
    filename = root + filename;

    if (!m_file.is_open()){
        m_file.open( filename.c_str(),std::ios::in|std::ios::binary);
        if (!m_file.is_open())
            return 404;
    }

    LOG(INFO) << "Start Offset:" << m_rangeStart << std::endl;

    m_file.seekg(m_rangeStart);
    int bufSize = 4096*100;
    char buf[bufSize+4];
    int offset = 0;
    size_t totalSize = 0;
    char t;
    int retVal = 0;
    try{
        while (m_file.get(t))
        {
            buf[offset++] = t;
            if(offset==bufSize){
                retVal = m_socket.putMessage(buf, offset);
                if(retVal != 0 ){
                    LOG(INFO) << "putMessage Error" << std::endl;
                    return -1;
                }
                totalSize += offset;
                offset = 0;
                memset(buf, 0, bufSize);
            }
        }
        LOG(INFO) << "putMessage Final(" << m_socket.getSocket() << "):" << offset << std::endl;
        if(offset != 0)
            m_socket.putMessage(buf, offset);

    } catch(std::runtime_error& e) {
        LOG(INFO) << e.what() << std::endl;
    } catch(...){
        LOG(INFO) << "putMessage Error" << std::endl;
    }
    //LOG(INFO) << "close fil" << std::endl;
    //file.close();
    return 200;
}

int ProtocolHttp::sendResponse(){

    size_t bodySize = 0;
    int retVal = _GetBodySize(bodySize);
    if(retVal != 200 )
        return _ResponseError(retVal);

    std::string header;
    if(m_isRange)
        header = "HTTP/1.1 206 Partial Content\r\n";
    else
        header = "HTTP/1.1 200 OK\r\n";
    std::time_t result = std::time(nullptr);
    header += std::string("Date: ") + std::asctime(std::localtime(&result));
    header += "Server: ho80\r\n";
    header += "Accept-Ranges: bytes\r\n";
    header += std::string("Content-Length: ") + std::to_string(bodySize - m_rangeStart)+ "\r\n";
    if(m_isRange)
        header += std::string("Content-Range: bytes ") + std::to_string(m_rangeStart) + "-" + std::to_string(bodySize-1) + "/" + std::to_string(bodySize) + "\r\n";
    if(m_requestPath.find(".ico") != std::string::npos)
        header += "Content-Type: image/vnd.microsoft.icon\r\n";
    else if(m_requestPath.find(".jpeg") != std::string::npos)
        header += "Content-Type: image/jpeg\r\n";
    else if(m_requestPath.find(".jpg") != std::string::npos)
        header += "Content-Type: image/jpeg\r\n";
    else if(m_requestPath.find(".mkv") != std::string::npos)
        header += "Content-Type: application/octet-stream\r\n";
    else if(m_requestPath.find(".mp4") != std::string::npos)
        header += "Content-Type: video/mp4\r\n";
    else
        header += "Content-Type: text/html\r\n";
    header += "Connection: Closed\r\n";
    header += "\r\n";
    //try{
        m_socket.putMessage(header.c_str(), header.size());
        if(_SendBody() == -1 )
            return -1;
        m_socket.putClose();
   //} catch(std::runtime_error& e) {
   //    std::cerr << e.what() << std::endl;
   //} catch(...) {
   //    std::cerr << "Unknown exception: sendResponse()" << std::endl;
   //}

    return 0;
}

int ProtocolHttp::_ReadLine(const char* buf, size_t size){
    size_t pos = 0;
    while(pos < size){
        if(*(buf+pos) == '\r' 
            && *(buf+pos+1) == '\n'){
            return pos;
        }
        pos++;
    }
    return -1;
}

int ProtocolHttp::_ParseRequestHeader(const char* buf, size_t size){
    char line[2048];
    size_t pos = 0, offset = 0;
    memset(line, 0, 2048);
    ///1 line
    {
        pos = _ReadLine(buf, size);
        memcpy(line, buf, pos);
        offset = pos + 2;
        char method[16];
        char path[1024];
        char version[16];
        int c = sscanf(line,"%s %s %s", method, path, version);
        if( c != 3 ) return -1;
        m_requestMethod = method;
        LOG(INFO) << "ParseRequestHeader:" << m_requestMethod << std::endl;
        m_requestPath = path;
        LOG(INFO) << "ParseRequestHeader:" << m_requestPath << std::endl;
        m_requestVersion = version;
        LOG(INFO) << "ParseRequestHeader:" << m_requestVersion << std::endl;
    }
    ///options
    while(1){
        pos = _ReadLine(buf+offset, size-offset);
        if(pos <= 0)
            break;
        std::string str(buf+offset, buf+offset+pos);
        LOG(INFO) << str << std::endl;
        m_requestHeader.emplace_back(buf+offset, buf+offset+pos);
        offset += pos + 2;
        if(size <= offset)
            break;
    }
    LOG(INFO) << "ParseRequestHeader END" << std::endl;
    return 0;
}

int ProtocolHttp::_ResponseError(int errorcode){
    std::string errStr;
    switch(errorcode){
        case 400:
            errStr = "Bad Request"; //요청 자체가 잘못되었을때 사용하는 코드이다.
            break;
        case 401:
            errStr = "Unauthorized"; //인증이 필요한 리소스에 인증 없이 접근할 경우 발생한다. 이 응답 코드를 사용할 때에는 반드시 브라우저에게 어느 인증 방식[1]을 사용할 것인지 보내 주어야 한다. 단순히 권한이 없는 경우 이 응답 코드 대신 아래 403 Forbidden을 사용해야 한다.
            break;
        case 403:
            errStr = "Forbidden"; // 서버가 요청을 거부할 때 발생한다. 관리자가 해당 사용자를 차단했거나 서버에 index.html 이 없는 경우에도[2] 발생할 수 있다. 혹은 권한이 없을 때(로그인 여부와는 무관하다)에도 발생한다.
            break;
        case 404:
            errStr = "Not Found"; //찾는 리소스가 없다는 뜻이다.
            break;
        case 405:
            errStr = "Method Not Allowed"; //PUT이나 DELETE 등 서버에서 허용되지 않은 메소드로 요청시 사용하는 코드이다.
            break;
        case 406:
            errStr = "Not Acceptable" ; //요청은 정상이나 서버에서 받아들일 수 없는 요청일시 사용하는 코드이다. 보통 웹 방화벽에 걸리는 경우 이 코드가 반환된다.
            break;
        case 407:
            errStr = "Proxy Authentication Required"; // : 프록시 인증이 필요할 경우
            break;
        case 408:
            errStr = "Request Timeout"; // 요청 중 시간이 초과되었을때 사용하는 코드이다.
            break;
        case 409:
            errStr = "Conflict"; 
            break;
        case 410:
            errStr = "Gone"; //404와는 달리 찾는 리소스가 영원히 사라진 경우 사용하는 코드이다.
            break;
        case 411:
            errStr = "Length Required";
            break;
        case 412:
            errStr = "Precondition Failed";
            break;
        case 413:
            errStr = "Requested Entity Too Large"; // 요청 본문이 너무 긴 경우 발생한다. 서버 소프트웨어로 엔진엑스를 사용하는 경우 기본 설정 그대로 사용하면 큰 첨부파일을 올릴 때 이 오류 코드가 발생하게 된다.
            break;
        case 414:
            errStr = "Requested URL Too Long"; // URL이 너무 길 때 발생한다.
            break;
        case 415:
            errStr = "Unsupported Media Type";
            break;
        case 416:
            errStr = "Requested Range Not Satisfiable"; //: 요청 헤더의 Range로 지정한 범위가 잘못되었을 때 발생한다.
            break;
        case 417:
            errStr = "Expectation Failed";
            break;
        case 418:
            errStr = "I'm a teapot"; // 찻주전자로 커피를 만들 수 없음. 하이퍼텍스트 커피 포트 제어 프로토콜(HTCPCP)(RFC 2324)[3]에서 사용되는 코드이다.
            break;
        case 429:
            errStr = "Too Many Requests"; // 일정 시간 동안 너무 많은 요청을 보냈을 때 이를 거부하기 위해 사용한다. 나무위키에서는 페이지 소스(/raw/문서명) 보기를 너무 자주 요청했을 때 임시 차단을 걸기 위해 사용한다.
            break;
        case 451:
            errStr = "Unavailable For Legal Reasons";
            break;
        default:
            errorcode = 500;
            errStr = "Internal Server Error";
    }

    std::string header;
    header = "HTTP/1.1 ";
    header += std::to_string(errorcode);
    header += " ";
    header += errStr + "\r\n";
    std::time_t result = std::time(nullptr);
    header += std::string("Date: ") + std::asctime(std::localtime(&result));
    header += "Server: ho80\r\n";
    header += std::string("Content-Length: ") + std::to_string(errStr.size())+ "\r\n";
    header += "Content-Type: text/html\r\n";
    header += "Connection: Closed\r\n";
    header += "\r\n";
    m_socket.putMessage(header.c_str(), header.size());
    m_socket.putMessage(errStr.c_str(), errStr.size());

    return 0;
}
