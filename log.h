#ifndef log_h
#define log_h

#include <string>

class Log{
public:

    static void clear_error_log();

    static void add_error(std::string message,bool allow_save=true);

    static void add_log(std::string message);
};

#endif
