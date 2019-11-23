#ifndef __UTIL_LOGGER__
#define __UTIL_LOGGER__

#include <memory>
#include <string>
#include <cstdio>
#include <cstdarg>
#include <ctime>

using namespace std;

class Logger {
private:
    static const size_t BufferSize = 5 * 1024 * 1024;
    FILE *debugf;
    FILE *infof;
    FILE *errorf;
    char info_buf[2 * BufferSize] = {0};
    char error_buf[2 * BufferSize] = {0};
    size_t info_buf_size = 0;
    size_t error_buf_size = 0;
    string debug_prefix = "[DEBUG] ";
    string info_prefix;
    string error_prefix;
    bool debug_time_flag = true;
    bool info_time_flag = false;
    bool error_time_flag = false;

    /**
     * Flushes the current log buffer
     * @param fout file handler to flush
     * @param buffer_size current size of the used buffer
     * @param buffer current buffer
     */
    static void flush(FILE *fout, size_t &buffer_size, char *buffer) {
        fwrite(buffer, sizeof(char), buffer_size, fout);
        fflush(fout);
        buffer_size = 0;
    }

    /**
     * Flushes the buffer if it is full
     * @param fout fout file handler to dump
     * @param buffer_size current size of the used buffer
     * @param buffer current buffer
     */
    static void dump(FILE *fout, size_t &buffer_size, char *buffer) {
        if (buffer_size >= BufferSize) {
            flush(fout, buffer_size, buffer);
        }
    }


    Logger() : debugf(stderr), infof(stdout), errorf(stderr) {}

public:
    ~Logger() {
        flush(errorf, error_buf_size, error_buf);
        flush(infof, info_buf_size, info_buf);

        if (debugf != stderr) {
            fclose(debugf);
        }
        if (infof != stdout) {
            fclose(infof);
        }
        if (errorf != stderr) {
            fclose(errorf);
        }
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
    inline void error(const char *format, ...) {
        va_list args;
        va_start(args, format);
        if (error_time_flag) {
            time_t tt = time(NULL);
            char *tt_str = ctime(&tt);
            tt_str[strlen(tt_str)-1]='\0';
            error_buf_size += sprintf(error_buf + error_buf_size, "%s ",tt_str);
        }
        error_buf_size += sprintf(error_buf + error_buf_size, "%s", error_prefix.c_str());
        error_buf_size += vsprintf(error_buf + error_buf_size, format, args);
        va_end(args);
        dump(errorf, error_buf_size, error_buf);
    }

};

#endif
