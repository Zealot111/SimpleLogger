// Copyright (C) 2020 zealot111(at)rambler.ru MIT LICENSE

#ifndef _SIMPLELOGGER_H_
#define _SIMPLELOGGER_H_

#if defined(_MSC_VER)
#define SLOG_FUNCTION __FUNCTION__
#elif defined(__GNUC__) 
#define SLOG_FUNCTION __PRETTY_FUNCTION__
#endif

#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <mutex>
#include <memory>
#include <unordered_map>
#include <atomic>


#define CLOG(l,x) s_logger::log_writer(l, x, __FILE__, SLOG_FUNCTION, __LINE__)
#define LOG(x) CLOG("default",x)
#define CLOGF(l,x,f, ...) s_logger::log_writer(l, x, __FILE__, SLOG_FUNCTION, __LINE__).printf(f, __VA_ARGS__)
#define LOGF(x,f, ...) s_logger::log_writer("default", x, __FILE__, SLOG_FUNCTION, __LINE__).printf(f, __VA_ARGS__)

namespace s_logger {

    enum LOG_LEVELS {
        NO_LOG_LEVEL = 0,
        TRACE = 1,
        DEBUG = 2,
        INFO = 3,
        WARNING = 4,
        ERROR = 5,
        FATAL = 6
    };

    void create_logger(const std::string& name = "default", const bool echo_cout = true, const std::string& filename = "", int min_log_level = 0, const std::string& msg_string = " %datetime %thread %level [%file:%line:%func]");
    void remove_logger(const std::string& name);

    extern std::mutex write_log_mutex;

    class logger {
        friend class log_writer;
        friend void create_logger(const std::string& name, const bool echo_cout, const std::string& filename, int min_log_level, const std::string& msg_string);

        std::ofstream logstream;
        int flush_each_count = 0;

        void flush_str_to_log(int log_level, const char* file, const char* func, int line);
        std::ostringstream str;
        
    public:
        std::atomic<int> min_log_level = 0;
        std::string msg_string = "%datetime %thread %level [%file:%line:%func]";
        std::string filename;
        
        bool echo_cout = true;
        int flush_each_n = 50;

        void create_log_file();
        logger(const bool echo_cout, const std::string& filename, int min_log_level, const std::string& msg_string);
        ~logger();

    public:

        logger(logger&& right) noexcept = delete;
        logger& operator=(logger&& right) noexcept = delete;
        logger& operator=(const logger& right) = delete;

    };

    class log_writer {
        const char* _logname, * _file, * _func;
        int _log_level, _line;
        logger& _l;
        bool _allowed_to_write;

    public:
        log_writer(const char* logname, int log_level, const char* file, const char* func, int line);
        ~log_writer();

        template <typename T>
        log_writer& operator<<(T&& t) {
            if (!_allowed_to_write) return *this;
            std::lock_guard<std::mutex> lock(write_log_mutex);
            _l.str << " " << std::forward<T>(t);
            return *this;
        }
        log_writer& operator<<(std::ostream& (*t)(std::ostream&));

        log_writer& printf(const char* format, ...);
    };

    std::string& get_current_time(std::string& ctime, const std::string& d_date = "", const std::string& d_datetime = " ", const std::string& d_time = "", int ms_len = 0);

    std::string& fill_filename_template(std::string& filename);

    std::string& fill_msg_template(std::string& to_str, const char* file, int line, const char* func, int level);


    namespace strutils {
        std::string& rtrim(std::string& str, const std::string& symbols = " \f\n\r\t\v");

        std::string& replace(std::string& str, const char* from, const char* to = "");

    }

    extern std::unordered_map<std::string, std::unique_ptr<logger>> loggers;
    
}










#endif 

