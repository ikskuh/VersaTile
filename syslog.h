#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <limits>

namespace syslog
{
    enum class loglevel : unsigned int
    {
        nothing = 0,
        error = 100,
        warning = 200,
        message = 300,
        everything = std::numeric_limits<unsigned int>::max(),
    };

    static inline bool operator <(loglevel lhs, loglevel rhs)
    {
        return static_cast<unsigned int>(lhs) < static_cast<unsigned int>(rhs);
    };

    extern loglevel level;

    template<typename T>
    void raw(const T & arg)
    {
        std::cerr << arg;
    }

    template<typename T, typename... Args>
    void raw(const T & val, const Args &... args)
    {
        raw(val);
        raw(args...);
    }

    template<typename... Args>
    void error(const Args &... args)
    {
        if(level < loglevel::error) return;
        raw("[ERR] ");
        raw(args...);
        raw("\n");
    }

    template<typename... Args>
    void warning(const Args &... args)
    {
        if(level < loglevel::warning) return;
        raw("[WARN] ");
        raw(args...);
        raw("\n");
    }

    template<typename... Args>
    void message(const Args &... args)
    {
        if(level < loglevel::message) return;
        raw("[MSG] ");
        raw(args...);
        raw("\n");
    }

    template<typename... Args>
    void write(loglevel lv, const Args &... args)
    {
        if(level < lv) return;
        raw("[MSG] ");
        raw(args...);
        raw("\n");
    }
}

#endif // LOG_H
