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

#ifndef SRC_AGENT_CURLEASYWRAPPER_H_
#define SRC_AGENT_CURLEASYWRAPPER_H_

#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "log/log.h"
#include "err/err.h"
#include "curl/curl.h"

namespace log2esd {
  namespace agent {
    using std::string;
    using std::vector;

    class CURLEasyWrapper {
     public:
       typedef vector<char> MALLOC;

     public:
       struct Buffer {
         MALLOC * data_;
         size_t * size_;
       };

     public:
       explicit CURLEasyWrapper(int size = 1024*1024);
       virtual ~CURLEasyWrapper();

     public:
       int HttpPost(const string & url, const string & data, char ** recv = NULL, size_t * size = NULL);
       int HttpGet(const string & url, char ** data = NULL, size_t * size = NULL);

     public:
       static size_t WriteCallback(char *ptr, size_t size, size_t nmemb, void *data);

     public:
       void Destroy();

     private:
       const CURLEasyWrapper & operator=(const CURLEasyWrapper &);
       CURLEasyWrapper(const CURLEasyWrapper &);

     private:
       CURL * handle_;
       MALLOC mem_;
    };
  }  // namespace agent
}  // namespace log2esd

#endif  // SRC_AGENT_CURLEASYWRAPPER_H_
