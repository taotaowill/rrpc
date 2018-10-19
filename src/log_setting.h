#ifndef RRPC_LOG_SETTING_H_
#define RRPC_LOG_SETTING_H_

#include <string>

#include "glog/logging.h"
#include "gflags/gflags.h"

void SetupLog(const std::string name) {
    if (FLAGS_log_dir.empty()) {
        FLAGS_log_dir = ".";
    }
    std::string info_f = FLAGS_log_dir + "/" + name + ".INFO.";
    std::string warning_f = FLAGS_log_dir + "/" + name + ".WARNING.";
    std::string error_f = FLAGS_log_dir + "/" + name + ".ERROR.";
    google::SetLogDestination(google::INFO, info_f.c_str());
    google::SetLogDestination(google::WARNING, warning_f.c_str());
    google::SetLogDestination(google::ERROR, error_f.c_str());

    google::SetLogSymlink(google::INFO, name.c_str());
    google::SetLogSymlink(google::WARNING, name.c_str());
    google::SetLogSymlink(google::ERROR,  name.c_str());
    google::InstallFailureSignalHandler();
}

#endif  // RRPC_LOG_SETTING_H_
