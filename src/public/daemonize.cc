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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <string>
#include "log/log.h"
#include "public/daemonize.h"

#ifdef __linux__
#include <sys/prctl.h>
#endif

namespace log2esd {
  namespace pub {
    int Daemon::Daemonize(int nochdir, int noclose, bool force_core, const char * lock_file, const char *run_as_user) {
      /* already a daemon */
      if (::getppid() == 1) {
        return 0;
      }

      /* run as daemon */
      // don't close standard files here. We will close them after file lock check
      int rc = ::daemon(nochdir, 1);
      if (rc) {
        LOG_ERROR("Failed to run as daemon, code=" << errno << " (" << ::strerror(errno) << ")");
        return EXIT_FAILURE; /* can not lock */
      }

      IgnoreSig();

      /* Create the lock file as the current user */
      if (lock_file && lock_file[0]) {
        int lfp = ::open(lock_file, O_RDWR | O_CREAT | O_TRUNC, 0640);
        if (lfp < 0) {
          LOG_ERROR("unable to create lock file " << lock_file << ", code=" \
                              << errno << " (" << ::strerror(errno) << ")");
          return EXIT_FAILURE;
        }
        // lock whole file
        struct flock stLock = { F_WRLCK, SEEK_SET, 0, 0, 0 };
        if (::fcntl(lfp, F_SETLK, &stLock) < 0) {
          LOG_ERROR("unable to lock the lock file " << lock_file \
                              << ", code=" << errno << " (" << ::strerror(errno) << ")");;
          return EXIT_FAILURE; /* can not lock */
        }

        /* write PID to lock file */
        char buf[16] = { 0 };
        ::snprintf(buf, sizeof(buf), "%u", ::getpid());
        ::write(lfp, buf, ::strlen(buf));
      }

      /* Drop user if there is one, and we were run as root */
      if (run_as_user && run_as_user[0] && (::getuid() == 0 || ::geteuid() == 0)) {
        struct passwd pd;
        struct passwd* pwdptr=&pd;
        struct passwd* tempPwdPtr;
        char pwdbuffer[200];
        int  pwdlinelen = sizeof(pwdbuffer);
        if (::getpwnam_r(run_as_user, pwdptr, pwdbuffer, pwdlinelen, &tempPwdPtr)) {
          if (::setuid(pd.pw_uid)) {
            LOG_ERROR("unable to switch user to " << run_as_user \
                                << ", code=" << errno << " (" << strerror(errno) << ")");
            return EXIT_FAILURE;
          }
        }
      }

      /* Redirect standard files to /dev/null */
      if (!noclose) {
        ::freopen("/dev/null", "r", stdin);
        ::freopen("/dev/null", "w", stdout);
        ::freopen("/dev/null", "w", stderr);
      }

#ifdef __linux__
      if (force_core) {
        struct rlimit corelimit;
        // set core size to unlimited
        corelimit.rlim_cur = RLIM_INFINITY;
        corelimit.rlim_max = RLIM_INFINITY;
        if (::setrlimit(RLIMIT_CORE, &corelimit) < 0) {
          LOG_ERROR("Cannot set core limits to unlimited, error = " \
                              << errno << " (" << ::strerror(errno) << ")");
          return EXIT_FAILURE;
        }

        // force setuid program to make a core dump
        if (::prctl(PR_SET_DUMPABLE, 1) < 0) {
          LOG_ERROR("Cannot enable core dumping, error = " \
                              << errno << " (" << ::strerror(errno) << ")");
          return EXIT_FAILURE;
        }
      }
#endif

      return 0;
    }

    int Daemon::WriteLockFile(const char * lock_file) {
      /* Create the lock file as the current user */
      if (lock_file && lock_file[0]) {
        int lfp = ::open(lock_file, O_RDWR | O_CREAT | O_TRUNC, 0640);
        if (lfp < 0) {
          LOG_ERROR("unable to create lock file " \
                              << lock_file << ", code=" << errno << " (" << ::strerror(errno) << ")");
          return EXIT_FAILURE;
        }
        // lock whole file
        struct flock stLock = { F_WRLCK, SEEK_SET, 0, 0, 0 };
        if (::fcntl(lfp, F_SETLK, &stLock) < 0) {
          LOG_ERROR("unable to lock the lock file " \
                              << lock_file << ", code=" << errno << " (" << ::strerror(errno) << ")");
          return EXIT_FAILURE; /* can not lock */
        }

        /* write PID to lock file */
        char buf[16] = { 0 };
        ::snprintf(buf, sizeof(buf), "%u", ::getpid());
        ::write(lfp, buf, ::strlen(buf));
      }

      return 0;
    }

    void Daemon::IgnoreSig() {
      ::signal(SIGTSTP, SIG_IGN);
      ::signal(SIGHUP, SIG_IGN);
      ::signal(SIGQUIT, SIG_IGN);
      ::signal(SIGPIPE, SIG_IGN);
      ::signal(SIGTTOU, SIG_IGN);
      ::signal(SIGTTIN, SIG_IGN);
      ::signal(SIGSTOP, SIG_IGN);
      ::signal(SIGTERM, SIG_IGN);
      ::signal(SIGINT, SIG_IGN);
    }
  }  // namespace pub
}  // namespace log2esd
