#include "glog/logging.h"

#include "message_header.h"
#include "pb_message_parser.h"

namespace rrpc {

RpcPbMessageParser::RpcPbMessageParser(muduo::string* buff) : buff_(buff) {
}

RpcPbMessageParser::~RpcPbMessageParser() {
}

int RpcPbMessageParser::Parse() {
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
                message_.src_id = header_.src_id;
                message_.dst_id = header_.dst_id;
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
            if(!message_.meta.ParseFromArray(buff_->c_str(), header_.meta_size)) {
                LOG(WARNING) << "invalid rpc meta";
                buff_->clear();
                stage_ = PARSING_HEADER;
                return -1;
            }

            LOG(INFO) << "meta parse ok"
                << ", sequence_id: " << message_.meta.sequence_id()
                << ", method: " << message_.meta.method();
            message_.data = buff_->substr(header_.meta_size, header_.data_size);
            LOG(INFO) << "data parse ok"
                << ", data: " << message_.data;
            *buff_ = buff_->substr(header_.meta_size + header_.data_size);
            message_list_.push_back(message_);
            stage_ = PARSING_HEADER;
            ret = 1;
        }
    }

    return ret;
}

RpcMessagePtr RpcPbMessageParser::GetMessage() {
    if (message_list_.size() > 0) {
        RpcMessage message = message_list_.front();
        message_list_.pop_front();
        return boost::make_shared<RpcMessage>(message);
    }

    return RpcMessagePtr();
}

}  // namespace rrpc
