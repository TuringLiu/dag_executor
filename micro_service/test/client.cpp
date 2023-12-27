#include "./gen-cpp/Serv.h"
#include </usr/local/include/thrift/transport/TSocket.h>
#include </usr/local/include/thrift/transport/TBufferTransports.h>
#include </usr/local/include/thrift/protocol/TBinaryProtocol.h>
#include <iostream>
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
 
//using boost::shared_ptr;
 
int main(int argc, char **argv) {
    std::shared_ptr<TSocket> socket(new TSocket("127.0.0.1", 9090)); 
    std::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    std::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    transport->open();
 
//*****************添加部分******************
    Student s;
    s.sno = 123;
    s.sname = "xiaoshe";
    s.ssex = 1;
    s.sage = 30;   
    ServClient client(protocol);
    std::cout<<"client send a data"<<std::endl;;
    client.put(s);
//*****************添加部分******************
    transport->close();
    return 0;
}