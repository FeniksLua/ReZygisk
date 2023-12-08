#include <cstdio>
#include <cstdlib>
#include <string_view>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/system_properties.h>

#include "main.hpp"
#include "utils.hpp"
#include "daemon.h"

using namespace std::string_view_literals;

int main(int argc, char **argv) {
    if (argc >= 2 && argv[1] == "monitor"sv) {
        init_monitor();
        return 0;
    } else if (argc >= 3 && argv[1] == "trace"sv) {
        if (argc >= 4 && argv[3] == "--restart"sv) {
            zygiskd::Init(getenv(MAGIC_PATH_ENV));
            zygiskd::ZygoteRestart();
        }
        auto pid = strtol(argv[2], 0, 0);
        if (!trace_zygote(pid)) {
            kill(pid, SIGKILL);
            return 1;
        }
        return 0;
    } else if (argc >= 2 && argv[1] == "ctl"sv) {
        if (argc == 3) {
            if (argv[2] == "start"sv) {
                send_control_command(START);
                return 0;
            } else if (argv[2] == "stop"sv) {
                send_control_command(STOP);
                return 0;
            } else if (argv[2] == "exit"sv) {
                send_control_command(EXIT);
                return 0;
            }
        }
        printf("Usage: %s ctl start|stop|exit\n", argv[0]);
        return 1;
    } else {
        LOGE("usage: %s monitor | trace <pid> | ctl <command>", argv[0]);
        return 1;
    }
}
