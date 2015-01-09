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

#include "producer/crawler.h"

namespace log2esd {
  namespace producer {
    void Crawler::LoadDirLogFiles() {
      struct dirent* ent = NULL;
      DIR * dir;
      char path[512];
      struct stat statbuf;
      if (NULL == (dir = ::opendir(log_home_.c_str()))) {
        LOG_ERROR("crawler load all log files failed! msg:" << log_home_ << " dir not exists");
        return;
      }

      while (NULL != (ent = ::readdir(dir))) {
        ::snprintf(path, sizeof(path),"%s/%s", log_home_.c_str(), ent->d_name);
        ::lstat(path, &statbuf);
        if (S_ISDIR(statbuf.st_mode)) {
          continue;
        }

        string file_name(ent->d_name);
        if (string::npos == file_name.find(".log")) {
          continue;
        }

        if (file_list_.end() != find(file_list_.begin(), file_list_.end(), ent->d_name)) {
          continue;
        }

        file_list_.push_back(ent->d_name);
      }

      ::closedir(dir);

      file_list_.sort();
    }

    const string & Crawler::GetCurrentFile() {
      return current_file_;
    }

    int Crawler::GetCurrentCursor() {
      return current_cursor_;
    }

    bool Crawler::SearchLog(bool is_first) {
      LOG_DEBUG("crawler search log files......");
      LoadDirLogFiles();

      if (!file_list_.size()) {
        return false;
      }

      if (is_first) {
        LOG_DEBUG("crawler search wriiten log files......");
        GetCurrentByWritten(written_file_);
        return GetNextLogFromWritten(current_file_);
      }

      if (current_file_.length()) {
        current_file_ = GetNextLogFromCurrent(current_file_);
      } else {
        current_file_ = file_list_.front();
      }

      return true;
      // find written file
    }

    int Crawler::FlushWritten(const string & file, int cursor) {
      char buf[1024] = {0};
      ::snprintf(buf, sizeof(buf) - 1, "%s,%d", file.c_str(), cursor);
      const string path = log_home_ + "/" + written_file_;
      ofstream os(path.c_str());
      if (os.is_open()) {
        os << buf;
        os.flush();
        os.close();
      } else {
        return -1;
      }

      return 0;
    }

    bool Crawler::GetNextLogFromWritten(const string & cur) {
      const string & min = file_list_.front();
      int result = ::strncasecmp(cur.c_str(), min.c_str(), cur.length());
      if (-1 == result) {
        current_file_ = min;
        current_cursor_ = 0;
        return true;
      }

      if (0 == result) {
        current_file_ = min;
        return true;
      }

      const string & max = file_list_.back();
      result = ::strncasecmp(cur.c_str(), max.c_str(), cur.length());
      if (1 == result) {
        current_file_ = "";
        current_cursor_ = 0;
        return false;
      }

      if (0 == result) {
        current_file_ = max;
        return true;
      }

      FILE_LIST::iterator begin = file_list_.begin();
      FILE_LIST::iterator end = file_list_.end();
      FILE_LIST::iterator it = find(begin, end, cur);
      if (it == file_list_.end()) {
        it = lower_bound(begin, end, cur);

        current_file_ = *it;
        current_cursor_ = 0;

        return true;
      }

      return true;
    }

    const string Crawler::GetNextLogFromCurrent(const string & cur) {
      FILE_LIST::iterator begin = file_list_.begin();
      FILE_LIST::iterator end = file_list_.end();
      FILE_LIST::iterator it = find(begin, end, cur);
      if (it == end) {
        it = lower_bound(begin, end, cur);
        return *it;
      }
      it++;
      if (it != end) {
        return *it;
      }

      return cur;
    }

    void Crawler::GetCurrentByWritten(const string & written) {
      if (!written.length()) {
        return;
      }

      const string path = log_home_ + "/" + written;
      int result = ::access(path.c_str(), F_OK);
      if (0 != result) {
        return;
      }

      ifstream fin(path.c_str());
      if (!fin.is_open()) {
        return;
      }

      char lines[1024] = {0};
      fin.getline(lines, sizeof(lines));
      if (0 >= ::strlen(lines)) {
        return;
      }

      StringSplit split(lines, ",");
      if (2 > split.TokenSize()) {
        LOG_WARN("crawler written file content is invalid");
        return;
      }

      current_file_ = split.GetToken(0);
      current_cursor_ = atoi(split.GetToken(1).c_str());

      fin.close();

      LOG_INFO("crawler .... current file:" << current_file_ << " cursor...." << current_cursor_);
    }
  }  // namespace producer
}  // namespace log2esd
