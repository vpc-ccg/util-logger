#include "logger.h"

int main() {
    Logger::instance().debug("Debug Level\n");
    Logger::instance().info("Info Level\n");
    Logger::instance().error("Error Level\n");
    Logger::instance().error.toggle_time().set_prefix("[ERROR] ");
    Logger::instance().error("Error Level with Prefix\n");

    Logger::instance().debug.set_file("debug.log");
    Logger::instance().debug.set_prefix("PREFIX ");
    Logger::instance().debug("SUFFIX\n");

    Logger::instance().debug.set_prefix("[HI THERE] ");

    Logger::instance().debug.set_file("debug.2.log");
    Logger::instance().debug("Hi Debug Level in File Two\n");
    Logger::instance().error.set_file("error.log");
    Logger::instance().error("Hi Error Level in File\n");
    Logger::instance().error.set_file("error.2.log");
    Logger::instance().error("Hi Error Level in File Two\n");
    Logger::instance().info.set_file("info.log");
    Logger::instance().info("Info Level in File\n");
    Logger::instance().info.set_file("info.2.log");
    Logger::instance().info("Info Level in File Two\n");
    return 0;
}
