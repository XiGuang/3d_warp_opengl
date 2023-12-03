//
// Created by Admin on 2023/11/30.
//

#include <glog/logging.h>
#include <gflags/gflags.h>
#include "module/application/application.h"

int main(int argc, char** argv){
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = true;  //是否打印到控制台
    FLAGS_alsologtostderr = true;  //打印到日志同时是否打印到控制台
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    Application app("../config/application.yaml");
    app.run();
}