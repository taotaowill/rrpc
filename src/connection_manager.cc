#include <string>

#include "connection_manager.h"

namespace rrpc {

bool RpcConnectionManager::Insert(RpcConnectionPtr conn) {
    MutexLock lock(&mutex_);
    conns_.insert(conn);
    return true;
}

void RpcConnectionManager::Remove(std::string conn_name) {
    MutexLock lock(&mutex_);
    RpcConnectionContainer::index<rpc_conn_name>::type::iterator it = \
            conns_.get<rpc_conn_name>().find(conn_name);
    if (it != conns_.get<rpc_conn_name>().end()) {
        (*it)->conn->forceClose();
        conns_.erase(conn_name);
    }
}

RpcConnectionPtr RpcConnectionManager::GetByName(std::string conn_name) {
   MutexLock lock(&mutex_);
   RpcConnectionContainer::index<rpc_conn_name>::type::iterator it = \
           conns_.get<rpc_conn_name>().find(conn_name);
   if (it != conns_.get<rpc_conn_name>().end()) {
       return *it;
   }

    return RpcConnectionPtr();
}

bool RpcConnectionManager::Exist(std::string conn_name) {
    MutexLock lock(&mutex_);
    RpcConnectionContainer::index<rpc_conn_name>::type::iterator it = \
            conns_.get<rpc_conn_name>().find(conn_name);
    return false;
}

}  // namespace rrpc
