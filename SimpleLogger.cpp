// Copyright (C) 2020 zealot111(at)rambler.ru MIT LICENSE

#include "SimpleLogger.h"

#include <ctime>
#include <iomanip>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <cstdarg>

namespace s_logger {
    using namespace std;

    class logger;

    const char* log_levels[] = {
        "NONE ","TRACE","DEBUG","INFO","WARN ","ERROR","FATAL"
    };

    inline const char* getLevelText(int level) {
        return log_levels[level];
    }

    std::unordered_map<std::string, std::unique_ptr<logger>> loggers;

    std::mutex write_log_mutex;
    
    logger::~logger() {
        logstream.flush();
        logstream.close();
    }

    void create_logger(const std::string& name, const bool echo_cout, const std::string& filename, int min_log_level, const std::string& msg_string) {
        std::lock_guard<std::mutex> lock(write_log_mutex);
        loggers.emplace(name, make_unique<logger>(echo_cout, filename, min_log_level, msg_string));
    }

    void remove_logger(const std::string& name) {
        std::lock_guard<std::mutex> lock(write_log_mutex);
        loggers.erase(name);
    }

    void logger::create_log_file() {
        std::lock_guard<std::mutex> lock(write_log_mutex);
        if (logstream.is_open()) {
            logstream.flush(); logstream.close();
        }
        string a_filename; fill_filename_template(a_filename);
        logstream.open(a_filename, std::ofstream::binary | std::ofstream::app);
    }

    logger::logger(const bool echo_cout, const std::string& filename, int min_log_level, const std::string& msg_string):
        min_log_level(min_log_level),
        msg_string(msg_string),
        filename(filename) {
        if (!filename.empty()) {
            string a_filename(filename); a_filename.reserve(128);
            fill_filename_template(a_filename);
            logstream.open(a_filename, std::ofstream::binary | std::ofstream::app);
        }
    }

    void logger::flush_str_to_log(int log_level, const char* file, const char* func, int line) {
        string tstr(msg_string); tstr.reserve(256);
        flush_each_count = (flush_each_count + 1) % flush_each_n;
        
        fill_msg_template(tstr, file, line, func, log_level);
        const std::string& s = str.str();
        logstream << tstr << s << "\n";
        if (echo_cout) {
            if (log_level < WARNING) {
                std::cout << tstr << s << std::endl;
            }
            else {
                std::cerr << tstr << s << std::endl;
            }
        }
        if (log_level >= WARNING || !flush_each_count) {
            logstream.flush();
        }
        str.str(std::string());
        str.clear();
    }

    log_writer::log_writer(const char* logname, int log_level, const char* file, const char* func, int line) :
        _logname(logname), _log_level(log_level), _file(file), _func(func), _line(line), _l(*loggers[logname].get())
    {
        _allowed_to_write = log_level >= _l.min_log_level;
    }

    log_writer::~log_writer() {
        if (!_allowed_to_write) return;
        std::lock_guard<std::mutex> lock(write_log_mutex);
        // call appropriate method of log
        _l.flush_str_to_log(_log_level, _file, _func, _line);
    }

    log_writer& log_writer::operator<<(std::ostream& (*t)(std::ostream&)) 
    {
        if (!_allowed_to_write) return *this;
        std::lock_guard<std::mutex> lock(write_log_mutex);
        t(_l.str);
        if (t == static_cast<std::ostream & (*)(std::ostream&)>(std::flush) || t == static_cast<std::ostream & (*)(std::ostream&)>(std::endl)) {
        }
        return *this;
    }

    log_writer& log_writer::printf(const char* format, ...)
    {
        if (!_allowed_to_write) return *this;
        std::lock_guard<std::mutex> lock(write_log_mutex);
#define BUFFER_LENGTH 256
        char buffer[BUFFER_LENGTH];
        char* dyn_buffer = NULL;
        va_list args;
        va_start(args, format);
        int d = vsnprintf(buffer, BUFFER_LENGTH, format, args);
        if (d >= BUFFER_LENGTH) {
            dyn_buffer = (char*)malloc((size_t)d + 1);
            d = vsnprintf(dyn_buffer, (size_t)d + 1, format, args);

        } else if (d < 0) {
            perror(buffer);
            _l.str << buffer;
        }
        va_end(args);
        if (dyn_buffer) {
            _l.str << dyn_buffer;
            free(dyn_buffer);
        }
        else {
            _l.str << buffer;
        }
#undef BUFFER_LENGTH
        return *this;
    }
    
     std::string &get_current_time(std::string &ctime, const std::string &d_date, const std::string& d_datetime, const std::string& d_time, int ms_len) {
        static const int mul[] = {1000000, 100000, 10000, 1000, 100, 10, 1};
        std::time_t t = std::time(nullptr);
        std::tm tm;
        localtime_s(&tm, &t);
        string format; format.reserve(100);
        format.append("%Y").append(d_date).append("%m").append(d_date).append("%d").append(d_datetime).append("%H").append(d_time).append("%M").append(d_time).append("%S");
        char time_str[80];
        strftime(time_str, sizeof(time_str), format.c_str(), &tm);
        if (ms_len == 0) {
            ctime = time_str;
            return ctime;
        }
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::chrono::system_clock::duration timepoint = now.time_since_epoch();
        timepoint -= std::chrono::duration_cast<std::chrono::seconds>(timepoint);

        char time_str2[100];
        unsigned int us = timepoint / std::chrono::microseconds(1) / mul[ms_len];
        std::snprintf(time_str2, sizeof(time_str2), "%s,%0*u", time_str, ms_len, static_cast<unsigned int>(us));
        ctime = time_str2;
        return ctime;
    }

     std::string& fill_filename_template(std::string& filename) {
        size_t pos = filename.find("%datetime");
        if (pos == std::string::npos) return filename;
        std::string time; time.reserve(100);
        get_current_time(time, "", "_", "");
        strutils::replace(filename, "%datetime", time.c_str());
        return filename;
    }

    std::string &fill_msg_template(std::string &to_str, const char *file, int line, const char *func, int level)
    {
        std::string time; time.reserve(100);
        strutils::replace(to_str, "%datetime", get_current_time(time, "-", " ", ":", 4).c_str());
        std::string s_file(file);
        strutils::replace(to_str, "%file", s_file.substr(s_file.find_last_of("\\/")+1, s_file.length()).c_str());
        strutils::replace(to_str, "%line", std::to_string(line).c_str());
        strutils::replace(to_str, "%func", func);
        ostringstream oss; oss << std::setw(5) << std::this_thread::get_id();
        strutils::replace(to_str, "%thread", oss.str().c_str());
        strutils::replace(to_str, "%level", getLevelText(level));
        return to_str;
    }

    namespace strutils {
        std::string& rtrim(std::string& str, const std::string& symbols) {
            return str.erase(str.find_last_not_of(symbols) + 1, str.length());
        }

        std::string& replace(std::string& str, const char* from, const char *to) {
            size_t p;
            size_t from_length = strlen(from);
            while ((p = str.find(from)) != std::string::npos) {
                str.replace(p, from_length, to);
            }
            return str;
        }


        // static string& ltrim(string& str, const string& symbols) {}
         //static string& trim(string& str, const string& symbols) {}

    }


#pragma endregion

          
}