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

#ifndef SRC_ERR_ERR_H_
#define SRC_ERR_ERR_H_

#include <errno.h>
#include <string>

namespace log2esd {
    enum {
      ERR_CURL_WRITE_BUFFER_TOO_SMALL = -100010,
      ERR_CURL_HANDLE_INVALID,
      ERR_CURL_SETOPT_URL_FAILED,
      ERR_CURL_SETOPT_POST_FAILED,
      ERR_CURL_SETOPT_WRITEDATA_FAILED,
      ERR_CURL_PERFORM_FAILED,

      ERR_THREAD_CREATE_FAILED = -100030,
      ERR_THREAD_CREATE_PIPE_FAILED,

      ERR_ELS_CREATE_INDEX_RESPONSE_INVALID = -100050,
      ERR_ELS_CREATE_INDEX_FAILED,
      ERR_ELS_PUSH_DOC_RESPONSE_INVALID,
      ERR_ELS_PUSH_DOC_NO_RESPONSE,
      ERR_ELS_PUSH_DOC_FAILED,
      ERR_ELS_PUSH_DOCS_FAILED,

      ERR_POOL_CACHE_FULL = -100060,
      ERR_POOL_CACHE_SPLIT_FULL,
      ERR_POOL_CACHE_ALL_FULL,
      ERR_POOL_CACHE_ALL_NOT_EMPTY,

      ERR_PIPE_CREATE_FAILED = -100070,
      ERR_PIPE_SET_NONBLOCK_FAILED,

      ERR_CAPTURE_LOAD_LOG_HOME_NOT_FOUND = -100080,
      ERR_CAPTURE_LOAD_WRITTEN_FILE_FAILED,
      ERR_CAPTURE_LOAD_NEW_FIN_FAILED,
      ERR_CAPTURE_LOAD_PARSE_INDEX_FAILED,
    };
}  // namespace log2esd

#endif  // SRC_ERR_ERR_H_
