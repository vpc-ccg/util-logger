#ifndef __UTIL_LOGGER__
#define __UTIL_LOGGER__

#include <memory>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <ctime>

using std::string;

class Logger;

template<size_t BufferSize>
class LogLane{

    char buffer[2 * BufferSize] = {0};

    bool use_time;
    string prefix;
    size_t buffer_index;
    FILE *file;

    inline void flush(){
        fwrite(buffer, sizeof(char), buffer_index, file);
        fflush(file);;
        buffer_index= 0;
    }

    inline void dump(){
        if (buffer_index >= BufferSize) {
            flush();
        }
    }
   
    template<class... Args>
    inline void operator()(const char *format, Args... args){
        if (BufferSize > 0){ 
            if(use_time){
                time_t tt = time(NULL);
                char *tt_str = ctime(&tt);
                tt_str[strlen(tt_str)-1]='\0';
                buffer_index += sprintf(buffer + buffer_index, "%s ",tt_str);
            }
            buffer_index += sprintf(buffer + buffer_index, "%s", prefix.c_str());
            buffer_index += sprintf(buffer + buffer_index, format, args...);

            dump();
        }
        else{
            if(use_time){
                time_t tt = time(NULL);
                char *tt_str = ctime(&tt);
                tt_str[strlen(tt_str)-1]='\0';
                buffer_index += sprintf(buffer + buffer_index, "%s ",tt_str);
            }
            fprintf(file, "%s", prefix.c_str());
            fprintf(file, format, args...);
            fflush(file);
        }
    }

    LogLane(FILE *file): use_time(false), prefix(""), buffer_index(0), file(file){}
    LogLane(): LogLane(stdout) {}

    friend class Logger;
};


class Logger {
private:

    static const size_t BufferSize = 5 * 1024 * 1024;


    LogLane<BufferSize> infolane;
    LogLane<BufferSize> errorlane;
    LogLane<0> debuglane;

    Logger() {}
public:
    ~Logger() {
        infolane.flush();
        errorlane.flush();
    }


    Logger(const Logger &) = delete;

    Logger &operator=(const Logger &) = delete;

    Logger(Logger &&) = delete;

    Logger &operator=(Logger &&) = delete;

    /**
     * Get an instance of the logger object using default stderr and stdout streams
     * @return Reference to Logger object instance.
     */
    static auto &instance() {
        static const std::unique_ptr <Logger> logger{new Logger()};
        //static Logger<BufferSize> logger;
        return *logger;
    }

    inline Logger &set_info(const std::string &str){
        if (info_buf_size > 0) {
            flush(infof, info_buf_size, info_buf);
        }
        infof = fopen(str.c_str(), "w+");
        if (infof == NULL) {
            fputs("Could not open the info stream\n", stderr);
            exit(-1);
        }
        return *this;
    }

    inline Logger &set_debug(const std::string &str) {
        debugf = fopen(str.c_str(), "w+");
        if (debugf == NULL) {
            fputs("Could not open the debug stream\n", stderr);
            exit(-1);
        }
        return *this;
    }

    inline Logger &set_error(const std::string &str){
        if (error_buf_size > 0) {
            flush(errorf, error_buf_size, error_buf);
        }
        errorf = fopen(str.c_str(), "w+");
        if (errorf == NULL) {
            fputs("Could not open the error stream\n", stderr);
            exit(-1);
        }
        return *this;
    }

    inline Logger &set_error_prefix(const std::string &prefix){
        error_prefix = prefix;
        return *this;
    }

    inline Logger &toggle_error_time() {
        error_time_flag  = ! error_time_flag;
        return *this;
    }



    /**
     *  Logger command to log the debug level information
     * @param format
     * @param ...
     */
    inline void debug(const char *format, ...) {
#ifdef DEBUG
        va_list args;
        va_start(args, format);
        fprintf(debugf, format, args);
        fflush(debugf);
        va_end(args);
#endif
    }

    /**
     * Logger command to log the info level information
     * @param format
     * @param ...
     */
    inline void info(const char *format, ...) {
        va_list args;
        va_start(args, format);
        info_buf_size += vsprintf(info_buf + info_buf_size, format, args);
        va_end(args);
        dump(infof, info_buf_size, info_buf);
    }

    /**
     * Logger command to log the error level information
     * @param format
     * @param ...
     */
    template<typename... Args>
    inline void error(const char *format, Args... args) {

        if (error_time_flag) {
            time_t tt = time(NULL);
            char *tt_str = ctime(&tt);
            tt_str[strlen(tt_str)-1]='\0';
            error_buf_size += sprintf(error_buf + error_buf_size, "%s ",tt_str);
        }
        error_buf_size += sprintf(error_buf + error_buf_size, "%s", error_prefix.c_str());
        error_buf_size += sprintf(error_buf + error_buf_size, format, args...);

        dump(errorf, error_buf_size, error_buf);
    }

};

#endif
