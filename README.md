# reverse rpc for service behind NAT

## requirements:
* boost
* protobuf
* muduo

## server:
* Start()
* Stop()
* RegisterServices()
* OnConnection()
* OnMessage()
* ParseMessage()
* ProcessRequest()

## client:
* Connect()
* Send()

## proxy:
* Start()
* Stop()
* OnConnection()
* OnMessage()
* ParseMessage()
* DispatchMessage()

## RoadMap:
* v0.0.1: pb-rpc service behind NAT with muduo(TCP)
* v0.1.0: udp/tcp tunneling
* v0.2.0: support multi-protocol(HTTP, PB, xml) within rrpc-protocol

