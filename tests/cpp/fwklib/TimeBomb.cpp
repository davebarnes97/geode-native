/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <chrono>
#include <fwklib/FwkLog.hpp>
#include <fwklib/TimeBomb.hpp>
#include <thread>

#include <geode/internal/geode_globals.hpp>

#include "config.h"

namespace apache {
namespace geode {
namespace client {
namespace testframework {

int32_t TimeBomb::svc() {
  while (!m_stop) {
    if (m_armed) {
      if (m_endOfLife < ACE_OS::gettimeofday()) {
        if (m_msg.length() > 0) {
          FWKSEVERE("Timeout failure has occurred, waited "
                    << m_seconds << " seconds. " << m_msg);
        } else {
          FWKSEVERE("Timeout failure has occurred, waited " << m_seconds
                                                            << " seconds.");
        }

#if defined(_SOLARIS)

        int32_t pid = ACE_OS::getpid();
        char buf[8192];
        sprintf(buf, "bash -c \"pstack %d\"", pid);
        FILE* pip = popen(buf, "r");
        if (!pip) {
          FWKSEVERE("TimeBomb: Unable to dump threads.");
        } else {
          std::this_thread::sleep_for(std::chrono::seconds(10));

          std::string dump;
          buf[0] = 0;
          while (fgets(buf, 8192, pip)) {
            dump.append(buf);
            buf[0] = 0;
          }
          dump.append(buf);
          pclose(pip);

          FWKSEVERE("TimeBomb reports these threads in process:"
                    << dump << "End of dump.\n");
        }

#endif

#if defined(_LINUX)

        int32_t pid = ACE_OS::getpid();
        char buf[8192];
        sprintf(buf,
                "bash -c \"perl $GEODE_NATIVE_HOME/../framework/scripts/gdb.pl "
                "%d ; cat "
                "gdbout.%d\"",
                pid, pid);
        FILE* pip = popen(buf, "r");
        if (!pip) {
          FWKSEVERE("TimeBomb: Unable to dump threads.");
        } else {
          std::this_thread::sleep_for(std::chrono::seconds(10));

          std::string dump;
          buf[0] = 0;
          while (fgets(buf, 8192, pip)) {
            dump.append(buf);
            buf[0] = 0;
          }
          dump.append(buf);
          pclose(pip);

          FWKSEVERE("TimeBomb reports these threads in process:"
                    << dump << "End of dump.\n");
        }

#elif defined(_WIN32)

        int32_t pid = ACE_OS::getpid();
        char buf[8192];

        sprintf(buf,
                "bash -c \"perl $GEODE_NATIVE_HOME/../framework/scripts/cdb.pl "
                "%d\"",
                pid);
        FILE* pip = _popen(buf, "r");
        if (!pip) {
          FWKSEVERE("TimeBomb: Unable to dump threads.");
        } else {
          std::this_thread::sleep_for(std::chrono::seconds(20));

          std::string dump;
          buf[0] = 0;
          while (fgets(buf, 8192, pip)) {
            dump.append(buf);
            buf[0] = 0;
          }
          dump.append(buf);
          _pclose(pip);

          FWKSEVERE("TimeBomb reports these threads in process:"
                    << dump << "End of dump.\n");
        }

#endif

        FWKSEVERE("Will now abort by calling exit( " << m_exitCode << " ).");
        exit(m_exitCode);
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  return 0;
}

}  // namespace testframework
}  // namespace client
}  // namespace geode
}  // namespace apache
