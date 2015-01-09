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

#ifndef SRC_PUBLIC_DEAMONIZE_H_
#define SRC_PUBLIC_DEAMONIZE_H_

namespace log2esd {
  namespace pub {
    class Daemon {
     public:
       static int Daemonize(int nochdir, int noclose, bool force_core, const char * lock_file = NULL, const char *run_as_user = NULL);
       static int WriteLockFile(const char * lock_file);
       static void IgnoreSig();
    };
  }  // namespace pub
}  // namespace log2esd

#endif  // SRC_PUBLIC_DEAMONIZE_H_
