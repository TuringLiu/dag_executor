#include "./gen-cpp/AddService.h"
#include <iostream>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>
#include <thrift/transport/TBufferTransports.h>

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace std;

int main(int argc, char **argv)
{
    shared_ptr<TSocket> socket(new TSocket("127.0.0.1", 9090));
	shared_ptr<TTransport> transport(new TBufferedTransport(socket));	
	shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	// tutorial::Test test;

    //申明一个AddOptClient（AddOpt为thrift定义中的AddOpt，这个结构是固定：xxClient,其中xx为thrift文件中的申明  的名）请求对象client
	tutorial::AddServiceClient client(protocol);
    transport->open();
	cout << "Client sent Add request from C++"<<endl;
	int sum_ = client.add(10, 1);//发送请求并等待接受结果
	cout << "10+1= " << sum_<< endl;
	transport->close();
}