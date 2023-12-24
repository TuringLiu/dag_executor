import sys
sys.path.append('gen-py')

from add import AddService

from thrift.transport import TSocket
from thrift.transport import TTransport
from thrift.protocol import TBinaryProtocol
from thrift.server import TServer

class AddServiceHandler(object):
	def add(self,Num1,Num2):
		print('Server receive Add request from Python')
		print ('Server Adding ', Num1, ' ', Num2)
		return int(Num1+Num2)


if __name__ == '__main__':
	handler = AddServiceHandler()#服务端最终处理类
	processor = AddService.Processor(handler)
	transport = TSocket.TServerSocket(host='127.0.0.1', port=9090)#本地地址
	tfactory = TTransport.TBufferedTransportFactory()
	pfactory = TBinaryProtocol.TBinaryProtocolFactory()
	server = TServer.TSimpleServer(processor, transport, tfactory, pfactory)
	
	# You could do one of these for a multithreaded server
	# server = TServer.TThreadedServer(
	#     processor, transport, tfactory, pfactory)
	# server = TServer.TThreadPoolServer(
	#     processor, transport, tfactory, pfactory)
	
	print('Starting the server...')
	server.serve()