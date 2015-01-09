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

#include "agent/els.h"

namespace log2esd {
  namespace agent {
    ELS::ELS(const string & addr):addr_(addr) {
    }

    int ELS::CheckError(const string & json) {
      size_t pos = json.find("\"errors\":");
      if (string::npos == pos) {
        return ERR_ELS_PUSH_DOC_NO_RESPONSE;
      }

      pos = pos + 9;
      if (0 != json.compare(pos, 5, "false")) {
        return ERR_ELS_PUSH_DOCS_FAILED;
      }

      LOG_DEBUG("write to els successed");

      return 0;
    }

    int ELS::PushDocs(const string & doc) {
      const string url = addr_ + "/_bulk";
      char * data = NULL;
      size_t size = 0;
      if (0 != curl_.HttpPost(url, doc, &data, &size)) {
        LOG_ERROR("http post response:" << data);
        return ERR_ELS_PUSH_DOC_NO_RESPONSE;
      }

      if (NULL == data || 0 >= size) {
        return ERR_ELS_PUSH_DOC_NO_RESPONSE;
      }

      if (1024 > size) {
        LOG_DEBUG("http post response:" << data << " size:" << size);        
      }
      if (100 < size) {
        data[100] = 0x00;
        size = 100;
      }

      return CheckError(data);
    }
  }  // namespace agent
}  // namespace log2esd
