#ifndef SRC_LOGGER_H_
#define SRC_LOGGER_H_

#include <Poco/Logger.h>
#include <string>
#include <utility>
#include <iostream>
#include <iostream>

#define LOGGER Poco::Logger::get(__PRETTY_FUNCTION__)

class Logger {
public:
    enum Level {
        LOG,
        WARN,
        ERROR,
        CRITICAL
    };
    Logger(const std::string &context)
        : context_(context)
    {}
    template <typename Arg, typename... Args>
    void log(Arg&& arg, Args&&... args)
    {
        logWithContext(LOG, context_, arg, &args...);
    }
    template <typename Arg, typename... Args>
    void warn(Arg&& arg, Args&&... args)
    {
        logWithContext(WARN, context_, arg, &args...);
    }
    template <typename Arg, typename... Args>
    static void logWithContext(Level level, const std::string &context, Arg&& arg, Args&&... args)
    {
        std::ostream &out {level == LOG ? std::cout : std::cerr};
        out << context << ": " << std::forward<Arg>(arg);
        ((out << ' ' << std::forward<Args>(args)), ...);
        out << std::endl << std::flush;
    }
private:
    std::string context_;
};



#endif // SRC_LOGGER_H_
