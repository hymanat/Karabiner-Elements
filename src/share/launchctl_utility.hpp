#pragma once

#include "apple_notification_center.hpp"
#include "constants.hpp"
#include <cstdlib>
#include <sstream>
#include <string>

namespace krbn {
class launchctl_utility final {
public:
  static void manage_session_monitor(void) {
    uid_t uid = geteuid();
    auto domain_target = (std::stringstream() << "gui/" << uid).str();
    auto service_target = (std::stringstream() << "gui/" << uid << "/org.pqrs.karabiner.karabiner_session_monitor").str();
    auto service_path = "/Library/LaunchAgents/org.pqrs.karabiner.karabiner_session_monitor.plist";

    {
      auto command = std::string("/bin/launchctl enable ") + service_target;
      system(command.c_str());
    }
    {
      auto command = std::string("/bin/launchctl bootstrap ") + domain_target + " " + service_path;
      system(command.c_str());
    }
    {
      auto command = std::string("/bin/launchctl enable ") + service_target;
      system(command.c_str());
    }
  }

  static void manage_console_user_server(bool load) {
    uid_t uid = geteuid();
    auto domain_target = (std::stringstream() << "gui/" << uid).str();
    auto service_target = (std::stringstream() << "gui/" << uid << "/org.pqrs.karabiner.karabiner_console_user_server").str();
    auto service_path = "/Library/LaunchAgents/org.pqrs.karabiner.karabiner_console_user_server.plist";

    if (load) {
      // If service_path is already bootstrapped and disabled, launchctl bootstrap will fail until it is enabled again.
      // So we should enable it first, and then bootstrap and enable it.

      {
        auto command = std::string("/bin/launchctl enable ") + service_target;
        system(command.c_str());
      }
      {
        auto command = std::string("/bin/launchctl bootstrap ") + domain_target + " " + service_path;
        system(command.c_str());
      }
      {
        auto command = std::string("/bin/launchctl enable ") + service_target;
        system(command.c_str());
      }

    } else {
      {
        auto command = std::string("/bin/launchctl bootout ") + domain_target + " " + service_path;
        system(command.c_str());
      }
      {
        auto command = std::string("/bin/launchctl disable ") + service_target;
        system(command.c_str());
      }

      apple_notification_center::post_distributed_notification(constants::get_distributed_notification_console_user_server_is_disabled());
    }
  }

  static void restart_console_user_server(void) {
    uid_t uid = geteuid();
    auto service_target = (std::stringstream() << "gui/" << uid << "/org.pqrs.karabiner.karabiner_console_user_server").str();

    auto command = std::string("/bin/launchctl kickstart -k ") + service_target;
    system(command.c_str());
  }
};
} // namespace krbn
