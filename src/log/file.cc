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

#include "log/file.h"

namespace log2esd {
  namespace log {
    FileHandle::FileHandle(const string & dir, int level):level_(level), log_dir_(dir), cur_file_("") {
      cur_file_ = log_dir_ + "/" + GetDateLog();
      os_.open(cur_file_.c_str(), ios_base::out|ios_base::ate|ios_base::app);
    }

    int FileHandle::GetLevel(const string & level) {
      int result = 2;
      if (0 == ::strncasecmp("INFO", level.c_str(), 4)) {
        result = 2;
      } else if (0 == ::strncasecmp("DEBUG", level.c_str(), 5)) {
        result = 1;
      } else if (0 == ::strncasecmp("WARN", level.c_str(), 5)) {
        result = 3;
      } else if (0 == ::strncasecmp("ERROR", level.c_str(), 5)) {
        result = 4;
      } else if (0 == ::strncasecmp("FATAL", level.c_str(), 5)) {
        result = 5;
      }

      return result;
    }

    void FileHandle::Logging(const string & level, const string & file, int line, const string & log) {
      int result = FileHandle::GetLevel(level);
      if (result < level_) {
        return;
      }

      ChangeOS();
      if (os_.good()) {
        os_ << "[" << level << "]";
        os_ << "[" << GetTime() << "]";
        os_ << "[" << ::getpid() << "]";
        os_ << "[" << ::pthread_self() << "]";
        os_ << "[file:" << file << "]";
        os_ << "[line:" << line << "]";
        os_ << "[" << log << "]" << endl;
      }
    }

    void FileHandle::ChangeOS() {
      if (!NeedChangeFile()) {
        return;
      }

      ReOpenOS();
    }

    const string FileHandle::GetTime() {
      time_t now = ::time(NULL);
      struct tm tm_now;
      ::localtime_r(&now, &tm_now);
      int year = tm_now.tm_year + 1900;
      int mon = tm_now.tm_mon + 1;
      int day = tm_now.tm_mday;
      int hour = tm_now.tm_hour;
      int min = tm_now.tm_min;
      int sec = tm_now.tm_sec;

      char date[1024] = {0};
      ::snprintf(date, sizeof(date), "%04d-%02d-%02d %02d:%02d:%02d", year, mon, day, hour, min, sec);

      return date;
    }

    void FileHandle::ReOpenOS() {
      if (os_.is_open()) {
        os_.close();
      }

      OpenOStream(cur_file_);
    }

    void FileHandle::OpenOStream(const string & file) {
      os_.open(file.c_str(), ios_base::out|ios_base::ate|ios_base::app);
    }

    const string FileHandle::GetDateLog() {
      time_t val = ::time(NULL);
      struct tm now;
      ::localtime_r(&val, &now);
      int year = now.tm_year + 1900;
      int mon = now.tm_mon + 1;
      int day = now.tm_mday;

      char date[1024] = {0};
      ::snprintf(date, sizeof(date), "%04d%02d%02d.log", year, mon, day);

      return date;
    }

    bool FileHandle::NeedChangeFile() {
      if (!os_.good()) {
        return true;
      }

      const string new_log = GetDateLog();
      if (0 != ::strncmp(new_log.c_str(), cur_file_.c_str(), cur_file_.length())) {
        cur_file_ = log_dir_ + "/" + new_log;
        return true;
      }

      return false;
    }
  }  // namespace log
}  // namespace log2esd
