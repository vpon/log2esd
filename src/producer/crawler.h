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

#ifndef SRC_PRODUCER_CRAWLER_H_
#define SRC_PRODUCER_CRAWLER_H_

#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <string>
#include <list>
#include <algorithm>
#include <fstream>
#include "public/split.h"
#include "log/log.h"
#include "err/err.h"

namespace log2esd {
  namespace producer {
    using std::string;
    using std::list;
    using std::find;
    using std::ofstream;
    using std::ifstream;
    using std::lower_bound;
    using log2esd::pub::StringSplit;

    class Crawler {
     public:
       typedef list<string> FILE_LIST;

     public:
       Crawler(const string & log_home = "./", const string & written_file = ".log2esd_written_file.txt"):current_cursor_(0), log_home_(log_home), written_file_(written_file), current_file_("") {}
       virtual ~Crawler() {
         file_list_.clear();
       }

     public:
       const string & GetCurrentFile();
       int GetCurrentCursor();

     public:
       bool SearchLog(bool is_sirst = false);

     public:
       int FlushWritten(const string & file, int cursor);

     private:
       void LoadDirLogFiles();
       void GetCurrentByWritten(const string & written);

     private:
       const string GetNextLogFromCurrent(const string & cur);
       bool GetNextLogFromWritten(const string & cur);

     private:
       const Crawler & operator=(const Crawler &);
       Crawler(const Crawler &);

     protected:
       FILE_LIST file_list_;
       int current_cursor_;
       string log_home_;
       string written_file_;
       string current_file_;
    };
  }  // namespace producer
}  // namespace log2esd

#endif  // SRC_PRODUCER_CRAWLER_H_
