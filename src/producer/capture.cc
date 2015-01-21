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

#include "producer/capture.h"

namespace log2esd {
  namespace producer {
    void Capture::FindLogs() {
      if (crawler_.SearchLog(true)) {
        ParseIndexAndType(crawler_.GetCurrentFile());
      }
    }

    int Capture::OpenLogFile(const string & file, int cursor) {
      const string path = log_home_ + "/" + file;
      int result = ::access(path.c_str(), F_OK);
      if (0 != result) {
        LOG_WARN("capture log file " << path << " not exist");
        return -1;
      }

      LOG_DEBUG("capture open log file:" << path << " cur pos:" << cursor);

      fd_ = ::fopen(path.c_str(), "r");
      if (NULL == fd_) {
        LOG_WARN("capture open " << path << " file failed");
      }

      if (0 != ::fseek(fd_, cursor_, SEEK_SET)) {
        LOG_WARN("capture seek cursor failed value:" << cursor);
      }

      return 0;
    }

    int Capture::EncodeCmd(char * lines, int size) {
      const string & cur = crawler_.GetCurrentFile();
      if (!cur.length()) {
        crawler_.SearchLog();
        return 0;
      }

      ParseIndexAndType(crawler_.GetCurrentFile());

      ::snprintf(lines, size - 1, "{\"index\":{\"_index\":\"%s\",\"_type\":\"%s\"}}\n", index_.c_str(), type_.c_str());
      return ::strlen(lines);
    }

    int Capture::GetLine(char * lines, int size) {
      const string & cur = crawler_.GetCurrentFile();
      if (!cur.length()) {
        crawler_.SearchLog();
        return 0;
      }

      if (0 != ::strncasecmp(cur.c_str(), open_file_.c_str(), cur.length())) {
        if (0 < open_file_.length()) {
          FlushStat(open_file_, cursor_);
          if (rm_) {
            RemoveLogFile(open_file_);
          }

          cursor_ = 0;
        } else {
          cursor_ = crawler_.GetCurrentCursor();
        }

        if (0 != OpenLogFile(cur, cursor_)) {
          return 0;
        }

        open_file_ = cur;
      }

      if (NULL == fd_) {
        if (0 != OpenLogFile(cur, cursor_)) {
          return 0;
        }
      }

      if (NULL == ::fgets(lines, size, fd_)) {
        fclose(fd_);
        fd_ = NULL;

        crawler_.SearchLog();

        return 0;
      }

      size = ::strlen(lines);
      cursor_ += size;

      return size;
    }

    int Capture::Flush() {
      return crawler_.FlushWritten(open_file_, cursor_);
    }

    int Capture::ParseIndexAndType(const string & file) {
      if (14 > file.length()) {
        time_t now = ::time(NULL);
        struct tm tm_now;
        ::localtime_r(&now, &tm_now);
        int year = tm_now.tm_year + 1900;
        int mon = tm_now.tm_mon + 1;
        int day = tm_now.tm_mday;
        int hour = tm_now.tm_hour;

        char index[32] = {0};
        ::snprintf(index, sizeof(index) - 1, "%04d%02d%02d%02d", year, mon, day, hour);
        char type[32] = {0};
        ::snprintf(type, sizeof(type), "%02d", hour);

        if (index_prefix_.length()) {
          index_ = index_prefix_ + "-" + index;          
        } else {
          index_ = index;
        }
        type_ = type;

        return 0;
      }

      LOG_DEBUG("capture parse index:" << file);

      if (index_prefix_.length()) {
        index_ = index_prefix_ + "-" + file.substr(0, 10);
      } else {
        index_ = file.substr(0, 10);
      }
      type_ = file.substr(8, 2);

      return 0;
    }

    void Capture::FlushStat(const string & open_file, int size) {
      if (!open_file.length()) {
        return;
      }

      const string path = log_home_ + "/" + ".written_stat.txt";
      ofstream os(path.c_str(), ios_base::out|ios_base::ate|ios_base::app);
      if (os.is_open()) {
        os << open_file << "," << size << endl;
        os.flush();
        os.close();
      }
    }

    void Capture::RemoveLogFile(const string & file) {
      if (!file.length()) {
        return;
      }

      const string cmd = "rm -f " + log_home_ + "/" + file;
      ::system(cmd.c_str());
    }
  }  // namespace producer
}  // namespace log2esd
