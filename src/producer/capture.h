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

#ifndef SRC_PRODUCER_CAPTURE_H_
#define SRC_PRODUCER_CAPTURE_H_

#include <stdio.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include "producer/crawler.h"
#include "log/log.h"
#include "err/err.h"

namespace log2esd {
  namespace producer {
    using std::string;
    using std::ios_base;
    using std::ofstream;
    using std::endl;

    class Capture {
     public:
       explicit Capture(const string & log_home, const string & written_file = ".log2esd_written_file.txt", bool rm = true):fd_(NULL), log_home_(log_home), written_file_(written_file), open_file_(""), index_(""), type_(""), cursor_(0), rm_(rm), crawler_(log_home, written_file) {}
       virtual ~Capture() {
         if (fd_) {
           ::fclose(fd_);
         }
       }

     public:
       void FindLogs();

     public:
       int GetLine(char * lines, int size);
       int Flush();
       int EncodeCmd(char * lines, int size);

     private:
       int ParseIndexAndType(const string & file);
       int OpenLogFile(const string & file, int cursor);
       void FlushStat(const string & open_file, int size);
       void RemoveLogFile(const string & file);

     private:
       const Capture & operator=(const Capture &);
       Capture(const Capture &);

     private:
       FILE * fd_;
       string log_home_;
       string written_file_;
       string open_file_;
       string index_;
       string type_;
       int cursor_;
       bool rm_;
       Crawler crawler_;
    };
  }  // namespace producer
}  // namespace log2esd

#endif  // SRC_PRODUCER_CAPTURE_H_
