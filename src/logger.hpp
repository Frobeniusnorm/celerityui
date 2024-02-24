#ifndef LOGGER_H
#define LOGGER_H
#include <experimental/source_location>
#include <iostream>
// 0 no logging, 1 only errors, 2 errors and warnings, 3 error, warnings and
// info, 4 verbose, 5 debugging messages for the library developers
static int logging_level = 5;
enum LOGTYPE { DEBUG, VERBOSE, INFO, ERROR, WARNING };
inline void log(LOGTYPE type, std::string msg,
                const std::experimental::source_location loc =
                    std::experimental::source_location::current()) {
  using namespace std;
  switch (type) {
  case DEBUG:
    if (logging_level >= 5)
      cout << "\033[0;33m[\033[0;32mDEBUG\033[0;33m]\033[0m " << msg << " [In "
           << loc.file_name() << ":" << loc.line() << "]" << endl;
    break;
  case VERBOSE:
    if (logging_level >= 4)
      cout << "\033[0;33m[\033[0;35mVERBOSE\033[0;33m]\033[0m " << msg << endl;
    break;
  case INFO:
    if (logging_level >= 3)

      cout << "\033[0;33m[\033[0;36mINFO\033[0;33m]\033[0m " << msg << endl;
    break;
  case WARNING:
    if (logging_level >= 2)
      cout << "\033[0;33m[\033[0;33mWARNING\033[0;33m]\033[0m " << msg
           << " [In " << loc.file_name() << ":" << loc.line() << "]" << endl;
    break;
  case ERROR:
    if (logging_level >= 1)
      cout << "\033[0;33m[\033[1;31mERROR\033[0;33m]\033[0m " << msg << " [In "
           << loc.file_name() << ":" << loc.line() << "]" << endl;
    throw std::runtime_error("error occured: " + msg);
  }
}
inline void info(std::string msg) noexcept { log(INFO, msg); }
inline void set_logger_level(int level) { logging_level = level; }
#endif
