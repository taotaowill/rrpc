#include "glog/logging.h"

#include "message_header.h"
#include "pb_request_parser.h"

namespace rrpc {

RpcPbRequestParser::RpcPbRequestParser(muduo::string* buff) : buff_(buff) {
}

RpcPbRequestParser::~RpcPbRequestParser() {
}

int RpcPbRequestParser::Parse() {
    int ret = 0;
    while (buff_->size() > 0) {
        if (stage_ == PARSING_HEADER) {
            LOG(INFO) << "buff_size: " << buff_->size();
            if (buff_->size() < RPC_MESSAGE_HEADER_SIZE) {
                return 0;
            }

            memcpy(&header_, buff_->c_str(), RPC_MESSAGE_HEADER_SIZE);
            if (!header_.Check()) {
                LOG(WARNING) << "invalid rpc message header";
                buff_->clear();
                return -1;
            } else {
                *buff_ = buff_->substr(RPC_MESSAGE_HEADER_SIZE);
                stage_ = PARSING_BODY;
                LOG(INFO) << "header parse ok"
                    << ", src_id: " << header_.src_id
                    << ", dst_id: " << header_.dst_id
                    << ", meta_size: " << header_.meta_size
                    << ", data_size: " << header_.data_size;
            }
        }

        if (stage_ == PARSING_BODY) {
            if (buff_->size() < header_.meta_size + header_.data_size) {
                return 0;
            }

            LOG(INFO) << "parse meta";
            if(!request_.meta.ParseFromArray(buff_->c_str(), header_.meta_size)) {
                LOG(WARNING) << "invalid rpc meta";
                buff_->clear();
                stage_ = PARSING_HEADER;
                return -1;
            }

            LOG(INFO) << "meta parse ok"
                << ", sequence_id: " << request_.meta.sequence_id()
                << ", method: " << request_.meta.method();
            request_.data = buff_->substr(header_.meta_size, header_.data_size);
            LOG(INFO) << "data parse ok"
                << ", data: " << request_.data;
            *buff_ = buff_->substr(header_.meta_size + header_.data_size);
            request_list_.push_back(request_);
            stage_ = PARSING_HEADER;
            ret = 1;
        }
    }

    return ret;
}

RpcRequestPtr RpcPbRequestParser::GetRequest() {
    if (request_list_.size() > 0) {
        RpcRequest request = request_list_.front();
        request_list_.pop_front();
        return boost::make_shared<RpcRequest>(request);
    }

    return RpcRequestPtr();
}

}  // namespace rrpc
