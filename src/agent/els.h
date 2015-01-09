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

#ifndef SRC_AGENT_ELS_H_
#define SRC_AGENT_ELS_H_

#include <string>
#include "agent/curleasywrapper.h"
#include "err/err.h"
#include "log/log.h"

namespace log2esd {
  namespace agent {
    using std::string;

    class ELS {
     public:
       ELS(const string & addr = "http://localhost:9200");
       virtual ~ELS() {}

     public:
       int PushDocs(const string & doc);

     private:
       int CheckError(const string & json);

     private:
       const ELS & operator=(const ELS &);
       ELS(const ELS &);

     private:
       string addr_;
       CURLEasyWrapper curl_;
    };
  }  // namespace agent
}  // namespace log2esd

#endif  // SRC_AGENT_ELS_H_
