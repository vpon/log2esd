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

#include "agent/curleasywrapper.h"

namespace log2esd {
  namespace agent {
    CURLEasyWrapper::CURLEasyWrapper(int size):handle_(NULL) {
      handle_ = curl_easy_init();

      if (CURLE_OK != curl_easy_setopt(handle_, CURLOPT_WRITEFUNCTION, CURLEasyWrapper::WriteCallback)) {
        LOG_ERROR("curleasywrapper set callback function failed");
      }

      mem_.reserve(size);
    }

    CURLEasyWrapper::~CURLEasyWrapper() {
      Destroy();
    }

    size_t CURLEasyWrapper::WriteCallback(char *ptr, size_t size, size_t nmemb, void *data) {
      size_t data_size = nmemb*size;
      Buffer * param = reinterpret_cast<Buffer *>(data);
      MALLOC * mem = param->data_;

      if (0 == *param->size_) {
        char * p = &((mem)->front());
        ::memcpy(p, ptr, data_size);
        *param->size_ = data_size;
      }

      return data_size;
    }

    int CURLEasyWrapper::HttpPost(const string & url, const string & data, char ** recv, size_t * size) {
      if (NULL == handle_) {
        LOG_ERROR("curleasywrapper handle is invalid");
        return -1;
      }

      if (CURLE_OK != curl_easy_setopt(handle_, CURLOPT_URL, url.c_str())) {
        LOG_ERROR("curleasywrapper http post set url failed");
        return ERR_CURL_SETOPT_URL_FAILED;
      }

      if (CURLE_OK != curl_easy_setopt(handle_, CURLOPT_POSTFIELDS, data.c_str())) {
        LOG_ERROR("curleasywrapper http post set data failed");
        return ERR_CURL_SETOPT_POST_FAILED;
      }

      Buffer recv_data;
      size_t read_size = 0;
      recv_data.data_ = &mem_;
      recv_data.size_ = &read_size;
      if (CURLE_OK !=  curl_easy_setopt(handle_, CURLOPT_WRITEDATA, (void *)&recv_data)) {
        LOG_ERROR("curleasywrapper set write ptr failed");
        return ERR_CURL_SETOPT_WRITEDATA_FAILED;
      }

      CURLcode res = CURLE_OK;
      if (CURLE_OK != (res = curl_easy_perform(handle_))) {
        LOG_ERROR("curleasywrapper http post data failed msg:" << curl_easy_strerror(res));
        return ERR_CURL_PERFORM_FAILED;
      }

      if (NULL != recv) {
        *recv = &mem_.front();
      }

      if (NULL != size) {
        *size = read_size;
      }

      return 0;
    }

    int CURLEasyWrapper::HttpGet(const string & url, char ** data, size_t * size) {
      if (NULL == handle_) {
        LOG_ERROR("curleasywrapper handle is invalid");
        return ERR_CURL_HANDLE_INVALID;
      }

      if (CURLE_OK != curl_easy_setopt(handle_, CURLOPT_URL, url.c_str())) {
        LOG_ERROR("curleasywrapper http get set url failed");
        return ERR_CURL_SETOPT_URL_FAILED;
      }

      Buffer recv_data;
      size_t read_size = 0;
      recv_data.data_ = &mem_;
      recv_data.size_ = &read_size;
      if (CURLE_OK !=  curl_easy_setopt(handle_, CURLOPT_WRITEDATA, (void *)&recv_data)) {
        LOG_ERROR("curleasywrapper set write ptr failed");
        return ERR_CURL_SETOPT_WRITEDATA_FAILED;
      }

      CURLcode res = CURLE_OK;
      if (CURLE_OK != (res = curl_easy_perform(handle_))) {
        LOG_ERROR("curleasywrapper http get data failed msg:" << curl_easy_strerror(res));
        return ERR_CURL_PERFORM_FAILED;
      }

      if (NULL != data) {
        *data = &mem_.front();
        (*data)[read_size] = 0x00;
      }

      if (NULL != size) {
        *size = read_size;
      }
      return 0;
    }

    void CURLEasyWrapper::Destroy() {
      if (NULL != handle_) {
        curl_easy_cleanup(handle_);
      }

      MALLOC().swap(mem_);
    }
  }  // namespace agent
}  // namespace log2esd
