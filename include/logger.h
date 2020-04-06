#ifndef __UTIL_LOGGER__
#define __UTIL_LOGGER__

#include <memory>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <ctime>

template<size_t MaxBufferSize>
class LogLane {
private:
    char buffer[2 * MaxBufferSize];
    size_t BufferSize = MaxBufferSize;
    bool use_time;
    std::string prefix;
    size_t buffer_index;
    FILE *file;
    bool is_std_stream;

    LogLane(FILE *file) : use_time(false), prefix(""), buffer_index(0), file(file), is_std_stream(true) {}

    LogLane() : LogLane(stdout) {}


    inline void flush() {
        if( BufferSize == 0){
            return;
        }
        fwrite(buffer, sizeof(char), buffer_index, file);
        fflush(file);;
        buffer_index = 0;
    }

    inline void dump() {
        if (buffer_index >= BufferSize) {
            flush();
        }
    }

    inline char *get_time() {
        time_t tt = time(NULL);
        char *tt_str = ctime(&tt);
        tt_str[strlen(tt_str) - 1] = '\0';
        return tt_str;
    }

public:
    ~LogLane() {
        flush();
        if (!is_std_stream) {
            fclose(file);
        }
    }

    inline LogLane &set_prefix(const std::string &pfix) {
        prefix = pfix;
        return *this;
    }

    inline LogLane &toggle_time() {
        use_time = !use_time;
        return *this;
    }

    inline LogLane &set_file(const std::string &filename) {
        flush();
        if (!is_std_stream) {
            fclose(file);
        }
        is_std_stream = false;
        file = fopen(filename.c_str(), "w+");
        if (file == NULL) {
            fputs("Could not open the debug stream\n", stderr);
            exit(-1);
        }
        return *this;
    }

    template<class... Args>
    inline void operator()(const char *format, Args... args) {
        if (BufferSize > 0) {
            if (use_time) {
                buffer_index += sprintf(buffer + buffer_index, "%s ", get_time());
            }
            buffer_index += sprintf(buffer + buffer_index, "%s", prefix.c_str());
            buffer_index += sprintf(buffer + buffer_index, format, args...);

            dump();
        } else {
            if (use_time) {
                fprintf(file, "%s ", get_time());
            }
            fprintf(file, "%s", prefix.c_str());
            fprintf(file, format, args...);
            fflush(file);
        }
    }

    inline void operator()(const char *format) {
        if (BufferSize > 0) {
            if (use_time) {

                buffer_index += sprintf(buffer + buffer_index, "%s ", get_time());
            }
            buffer_index += sprintf(buffer + buffer_index, "%s", prefix.c_str());
            buffer_index += sprintf(buffer + buffer_index, "%s", format);

            dump();
        } else {
            if (use_time) {
                buffer_index += fprintf(file, "%s ", get_time());
            }
            fprintf(file, "%s", prefix.c_str());
            fprintf(file, "%s", format);
            fflush(file);
        }
    }

    friend class Logger;
};


template<size_t BufferSize>
class DummyLogLane {
    inline void flush() {}

    inline void dump() {}

    DummyLogLane() {}

public:

    inline DummyLogLane &set_prefix(const std::string &pfix) {}

    inline DummyLogLane &toggle_time() {}

    inline DummyLogLane &set_file(const std::string &filename) {}

    template<class... Args>
    inline void operator()(const char *format, Args... args) {}

    inline void operator()(const char *format) {}

    friend class Logger;
};

class Logger {
private:
    static const size_t BufferSize = 5 * 1024 * 1024;

    Logger() {}

public:
    LogLane<BufferSize> info{stdout};
    LogLane<BufferSize> error{stderr};
#ifdef DEBUG
    LogLane<0> debug{stderr};
#else
    DummyLogLane<0> debug;
#endif

    ~Logger() {
        info.flush();
        error.flush();
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
        return *logger;
    }
};

#endif
