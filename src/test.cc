#include "logger.h"

int main(){
    Logger::instance().debug("Debug Level\n");
    Logger::instance().info("Info Level\n");
    Logger::instance().error("Error Level\n");
    Logger::instance().toggle_error_time().set_error_prefix("[ERROR] ").error("Error Level with Prefix\n");
    Logger::instance().error("YenYi\n");
    Logger::instance().set_debug("debug.log").debug("Hi Debug Level in File\n");
    Logger::instance().set_debug("debug.2.log").debug("Hi Debug Level in File Two\n");
    Logger::instance().set_error("error.log").error("Hi Error Level in File\n");
    Logger::instance().set_error("error.2.log").error("Hi Error Level in File Two\n");
    Logger::instance().set_info("info.log").info("Info Level in File\n");
    Logger::instance().set_info("info.2.log").info("Info Level in File Two\n");
    return 0;
}
