#ifndef __UTIL_LOGGER__
#define __UTIL_LOGGER__

#include <memory>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <ctime>



template<size_t BufferSize>
class LogLane{

    char buffer[2 * BufferSize];

    bool use_time;
    std::string prefix;
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

    LogLane(FILE *file): use_time(false), prefix(""), buffer_index(0), file(file){}
    LogLane(): LogLane(stdout) {}

    inline void set_prefix(const std::string &pfix){
	prefix = pfix;
    }

    inline void toggle_time(){
	use_time = !use_time;
    }

    inline void set_file(const std::string &filename){
	file = fopen(filename.c_str(), "w+");
        if (file == NULL) {
            fputs("Could not open the debug stream\n", stderr);
            exit(-1);
        }
    }
public:
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

    inline void operator()(const char *format){
        if (BufferSize > 0){ 
            if(use_time){
                time_t tt = time(NULL);
                char *tt_str = ctime(&tt);
                tt_str[strlen(tt_str)-1]='\0';
                buffer_index += sprintf(buffer + buffer_index, "%s ",tt_str);
            }
            buffer_index += sprintf(buffer + buffer_index, "%s", prefix.c_str());
            buffer_index += sprintf(buffer + buffer_index, "%s", format);

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
            fprintf(file, "%s", format);
            fflush(file);
        }
    }
    friend class Logger;
};


template<size_t BufferSize>
class DummyLogLane{

    inline void flush(){

    }

    inline void dump(){

    }

    inline void set_prefix(const std::string &pfix){

    }

    inline void toggle_time(){

    }

    inline void set_file(const std::string &filename){
    }
    
    DummyLogLane() {}

public:
    template<class... Args>
    inline void operator()(const char *format, Args... args){
    }

    inline void operator()(const char *format){
    }
    friend class Logger;
};
class Logger {
private:

    static const size_t BufferSize = 5 * 1024 * 1024;

    Logger() {}
public:

    LogLane<BufferSize> info;
    LogLane<BufferSize> error;
#ifdef DEBUG
    LogLane<0> debug;
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
        //static Logger<BufferSize> logger;
        return *logger;
    }

    inline Logger &set_info(const std::string &str){
        info.flush();
        info.set_file(str);
        return *this;
    }

    inline Logger &set_debug(const std::string &str) {
        debug.set_file(str);
        return *this;
    }

    inline Logger &set_error(const std::string &str){
        error.flush();
        error.set_file(str);
        return *this;
    }

    inline Logger &set_error_prefix(const std::string &prefix){
        error.set_prefix(prefix);
        return *this;
    }

    inline Logger &toggle_error_time() {
        error.toggle_time();
        return *this;
    }
};

#endif
