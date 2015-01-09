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

#ifndef SRC_LOG_FILE_H_
#define SRC_LOG_FILE_H_

#include <time.h>
#include <unistd.h>
#include <string>
#include <fstream>
#include "log/log.h"

namespace log2esd {
  namespace log {
    using std::string;
    using std::ofstream;
    using std::ios_base;
    using std::endl;

    class FileHandle {
     public:
       FileHandle(const string & dir, int level = 2);
       ~FileHandle() {}

     public:
       static FileHandle * Instance(const string & dir = "./", int level = 2) {
         static FileHandle * logger = NULL;
         if (NULL == logger) {
           logger = new FileHandle(dir, level);
         }

         return logger;
       }

     public:
       void Logging(const string & level, const string & file, int line, const string & log);

     private:
       void ChangeOS();
       void ReOpenOS();

     private:
       void OpenOStream(const string & file);

     private:
       const string GetDateLog();
       const string GetTime();
       bool NeedChangeFile();

     public:
       static int GetLevel(const string & level);

     private:
       const FileHandle & operator=(const FileHandle &);
       FileHandle(const FileHandle &);

     private:
       ofstream os_;
       int level_;
       string log_dir_;
       string cur_file_;
    };
  }  // namespace log
}  // namespace log2esd

#endif  // SRC_LOG_FILE_H_
