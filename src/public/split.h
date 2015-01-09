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

#ifndef SRC_PUBLIC_SPLIT_H_
#define SRC_PUBLIC_SPLIT_H_

#include <string.h>
#include <string>
#include <vector>

namespace log2esd {
  namespace pub {
    using std::string;
    using std::vector;

    class StringSplit {
    public:
      StringSplit(const string &str, const string &split) {
        token_.clear();
        char s1[10240] = {0};
        ::snprintf(s1, sizeof(s1) - 1, "%s", str.c_str());
        char * s2 = NULL;
        char * token = NULL;
        token = ::strtok_r(s1, split.c_str(), &s2);
        while (NULL != token) {
          token_.push_back(token);
          token = strtok_r(NULL, split.c_str(), &s2);
        }
      }

      ~StringSplit() {
        token_.clear();
      }

    public:
      int TokenSize() {
        return token_.size();
      }

      const string & GetToken(int index) {
        return token_[index];
      }

    private:
      vector<string> token_;
    };
  }  // namespace pub
}  // namespace log2esd

#endif  // SRC_PUBLIC_SPLIT_H_

