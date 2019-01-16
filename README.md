reverse rpc for service behind NAT

server:
    Start()
    Stop()
    RegisterServices()
    OnConnection()
    OnMessage()
    ParseMessage()
    ProcessRequest()

client:
    Connect()
    Send()

proxy:
    Start()
    Stop()
    OnConnection()
    OnMessage()
    ParseMessage()
    DispatchMessage()

TODO
    closure
    controller
