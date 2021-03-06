#pragma once

#include <map>
#include <string>

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/member.hpp"
#include "boost/multi_index/ordered_index.hpp"

#include "mutex.h"
#include "connection.h"

namespace rrpc {

using baidu::common::Mutex;
using baidu::common::MutexLock;
using boost::multi_index_container;
using namespace boost::multi_index;

struct rpc_conn_name {};   // index name
struct rpc_conn_id {};

typedef
    boost::multi_index_container<
        RpcConnectionPtr,
        indexed_by<
            ordered_unique<
                tag<rpc_conn_name>,
                BOOST_MULTI_INDEX_MEMBER(RpcConnection, std::string, conn_name)>,
            ordered_non_unique<
                tag<rpc_conn_id>,
                BOOST_MULTI_INDEX_MEMBER(RpcConnection, int32_t, conn_id)>
        >
    > RpcConnectionContainer;

class RpcConnectionManager {
public:
    RpcConnectionManager() {};
    bool Insert(RpcConnectionPtr conn);
    void Remove(std::string conn_name);
    bool Exist(std::string conn_name);
    RpcConnectionPtr Get(std::string conn_name);
    RpcConnectionPtr Get(int32_t conn_id);

private:
    Mutex mutex_;
    RpcConnectionContainer conns_;
};

}  // namespace rrpc
