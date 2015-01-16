/* ==================================================================================================
# Copyright 2014 vpon, Inc.
# --------------------------------------------------------------------------------------------------
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this work except in compliance with the License.
# You may obtain a copy of the License in the LICENSE file, or at:
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ================================================================================================== */

#include <unistd.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include "producer/capture.h"
#include "agent/els.h"
#include "public/daemonize.h"
#include "err/err.h"
#include "log/log.h"

using std::string;
using std::vector;
using std::ofstream;
using log2esd::producer::Capture;
using log2esd::agent::ELS;
using log2esd::pub::Daemon;
using log2esd::log::FileHandle;
using log2esd::ERR_ELS_PUSH_DOC_NO_RESPONSE;

struct AppConfig {
  string host_;
  string log_home_;
  string level_;
  string daemon_log_;
  string pid_file_;
  string written_file_;
  string rm_;
  int lines_;
  int line_size_;
  int interval_;
  AppConfig() : host_("localhost:9200"), log_home_("./log"), level_("INFO"), daemon_log_("/data/logs/log2esd/"), pid_file_(""), written_file_(".written_file.txt"), rm_("yes"), lines_(2), line_size_(2048), interval_(10) {}
};

void Help(char ** argv) {
  printf("##########################################\n\n");
  printf("Usage: %s -p <file.ini>"
         "\n"
         "Options:\n"
         "-h <192.168.0.1:9200> els service (default: localhost:9200)\n"
         "-s <app_logs> directory of app's logs (default: ./log)\n"
         "-n <lines> lines of read once (default: 2)\n"
         "-b <line_size> size of line (default: 2048)\n"
         "-i <interval> waiting interval (default: 2)\n"
         "-w <daemon log> directory of daemon logs (default: /data/logs/log2esd)\n"
         "-l <level> level of daemon logs (default: INFO)\n"
         "-r <yes/no> remove log file after send all datas (default yes)\n"
         "-d <pid_file> daemonize log2esd\n"
         "-H Show this help\n"
         "\n",
         argv[0]
         );
  printf("##########################################\n\n");
}

void GetParam(AppConfig * conf, int argc, char ** argv) {
  char opt;
  while (-1 != (opt = ::getopt(argc, argv, "h:s:d:n:b:i:w:l:r:H"))) {
    switch (opt) {
    case 'h':
      if (NULL != optarg) {
        conf->host_ = optarg;
      }
      break;

    case 's':
      if (NULL != optarg) {
        conf->log_home_ = optarg;
      }
      break;

    case 'd':
      if (NULL != optarg) {
        conf->pid_file_ = optarg;
      }
      break;

    case 'n':
      if (NULL != optarg) {
        conf->lines_ = ::atoi(optarg);
      }
      break;

    case 'b':
      if (NULL != optarg) {
        conf->line_size_ = ::atoi(optarg);
      }
      break;

    case 'i':
      if (NULL != optarg) {
        conf->interval_ = ::atoi(optarg);
      }
      break;

    case 'w':
      if (NULL != optarg) {
        conf->daemon_log_ = optarg;
      }
      break;

    case 'l':
      if (NULL != optarg) {
        conf->level_ = optarg;
      }
      break;

    case 'r':
      if (NULL != optarg) {
        conf->rm_ = optarg;
      }
      break;

    case 'H':
    default:
      Help(argv);
      ::_exit(0);
      break;
    }
  }

  printf("###########################################\n");
  printf("host:%s\n", conf->host_.c_str());
  printf("log home:%s\n", conf->log_home_.c_str());
  printf("read lines:%d\n", conf->lines_);
  printf("line size:%d\n", conf->line_size_);
  printf("waiting interval:%d\n", conf->interval_);
  printf("deamon log:%s\n", conf->daemon_log_.c_str());
  printf("log level:%s\n", conf->level_.c_str());
  printf("pid file:%s\n", conf->pid_file_.c_str());
  printf("remove log file: %s\n", conf->rm_.c_str());
  printf("###########################################\n");

  return;
}

int ReadLines(Capture & capture, int lines, char * block, int max_size) {
  int used = 0;
  while (lines) {
    int unused = max_size - used;
    int cmd_size = capture.EncodeCmd(block + used, unused);
    if (0 >= cmd_size) {
      break;
    }
    used += cmd_size;
    unused = max_size - used;
    int size = capture.GetLine(block + used, unused);
    if (0 < size) {
      used += size;
      unused = max_size - used;

      lines--;
    } else {
      used -= cmd_size;

      break;
    }
  }

  return used;
}

int PushToELS(ELS & els, const string & data) {
  return els.PushDocs(data);
}

int RetryPush(ELS & els, const string & data, int times, int interval) {
  while (times) {
    if (0 == PushToELS(els, data)) {
      return 0;
    }

    --times;

    LOG_WARN("retry push to els times " << times);

    ::sleep(interval);
  }

  return -1;
}

void LogToErrDoc(const string & log_home, const string & json) {
  uint now = ::time(NULL);
  ::srand(now);
  int random = ::rand()%1000;
  char file[1024] = {0};
  ::snprintf(file, sizeof(file) - 1, ".written_error_%u_%d.json", now, random);
  const string path = log_home + "/" + file;
  int result = ::access(path.c_str(), F_OK);
  if (0 == result) {
    LOG_WARN("agent log to err doc file name is exists");
  }

  ofstream os(path.c_str());
  if (os.is_open()) {
    os << json;
    os.flush();
    os.close();
  }
}

int main(int argc, char ** argv) {
  if (1 >= argc) {
    Help(argv);
    ::_exit(0);
  }

  AppConfig conf;
  GetParam(&conf, argc, argv);

  int block_size = conf.lines_*conf.line_size_;

  FileHandle * logger = FileHandle::Instance(conf.daemon_log_, FileHandle::GetLevel(conf.level_));
  if (NULL == logger) {
    printf("log2esd initial log failed");
    return -1;
  }
  LOG_DEBUG("log2esd initial log successed");

  if (conf.pid_file_.length()) {
    if (0 != Daemon::Daemonize(1, 1, false, conf.pid_file_.c_str())) {
      printf("log2esd daemonize failed");
      ::_exit(-1);
    }
  }
  LOG_DEBUG("log2esd daemonize successed");

  vector<char> mem(block_size);
  char * block = &mem.front();

  bool rm = true;
  if (0 == ::strncasecmp("no", conf.rm_.c_str(), 2)) {
    rm = false;
  }
  Capture capture(conf.log_home_, conf.written_file_, rm);
  LOG_DEBUG("log2esd initial capture successed");

  capture.FindLogs();
  LOG_DEBUG("log2esd search log files successed");

  ELS els(conf.host_);
  LOG_DEBUG("log2esd initial els successed");

  while (true) {
    int size = 0;
    if (0 >= (size = ReadLines(capture, conf.lines_, block, block_size))) {
      ::sleep(conf.interval_);
      continue;
    }

    block[size] = 0x00;

    if (5 > conf.lines_) {
      LOG_DEBUG("send to els:" << block);
    }
    int result = PushToELS(els, block);
    if (0 == result) {
      capture.Flush();
    } else if (ERR_ELS_PUSH_DOC_NO_RESPONSE == result) {
      if (0 != RetryPush(els, block, 3*600 , 2)) {
        LogToErrDoc(conf.log_home_, block);
      }
    } else {
      LOG_WARN("log to els failed data size:" << size);

      LogToErrDoc(conf.log_home_, block);

      ::sleep(60);
    }
  }

  vector<char>().swap(mem);

  delete logger;

  return 0;
}
