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

#ifndef SRC_LOG_LOG_H_
#define SRC_LOG_LOG_H_

#include <string>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <cstring>
#include "log/file.h"

namespace log2esd {
  namespace log {
    #define StreamLogging(level, expression)                \
      do {                                                  \
        using std::ostringstream;                           \
        using std::string;                                  \
        using std::cout;                                    \
        using std::endl;                                    \
        ostringstream out_os;                               \
        out_os << expression;                               \
        string const & out_string = out_os.str();           \
        cout << "[" << level << "][file:" << (::strrchr(__FILE__, '/')+1); \
        cout << " line:" << __LINE__ << "][" << out_string << "]" << endl; \
      } while (0)

    #define FileLogging(logger, level, expression)          \
      do {                                                  \
        using std::ostringstream;                           \
        using std::string;                                  \
        using std::cout;                                    \
        using std::endl;                                    \
        using log2esd::log::FileHandle;                     \
        ostringstream out_os;                               \
        out_os << expression;                               \
        string const & out_string = out_os.str();           \
        logger->Logging(level, (::strrchr(__FILE__, '/')+1), __LINE__, out_string); \
      } while(0)

    #define LOG_DEBUG(expression) FileLogging(FileHandle::Instance(), "DEBUG", expression)
    #define LOG_INFO(expression) FileLogging(FileHandle::Instance(), "INFO", expression)
    #define LOG_WARN(expression) FileLogging(FileHandle::Instance(), "WARN", expression)
    #define LOG_ERROR(expression) FileLogging(FileHandle::Instance(), "ERROR", expression)
    #define LOG_FATAL(expression) FileLogging(FileHandle::Instance(), "FATAL", expression)
  }
}

#endif  // SRC_LOG_LOG_H_
