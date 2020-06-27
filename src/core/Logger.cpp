//
// Created by sugoi on 17.01.19.
//

#include "Logger.h"
#include <sstream>
#include <cstdarg>

int logger_level = 4;
thread_local int logger::thread_id = 0;
int logger::highest_thread_id = 0;

void logger::register_logger_thread_id() {
  std::lock_guard<std::mutex> lock(print_mutex);
  highest_thread_id++;
  thread_id = highest_thread_id;
}

#ifdef __GNUG__

#include <cxxabi.h>
#include <stdlib.h>
#include <regex>

std::string demangle(const char* mangledName) {
  std::string result;
  std::size_t len = 0;
  int status = 0;
  char* ptr = __cxxabiv1::__cxa_demangle(mangledName, nullptr, &len, &status);
  if(status == 0) result = ptr; // hope that this won't throw
  else result = "demangle error";
  ::free(ptr);
  return result;
}

#else

std::string demangle(const char* mangledName) {
    return mangledName;
}

#endif

std::mutex logger::print_mutex;

#if LOGGER_LEVEL >= 1
void logError(const std::string& funcName, const std::string& fmt, ...) {
  if (logger_level < 1) {
    return;
  }

  va_list argptr;
  std::stringstream ss;
  ss << "[ERROR][" << logger::thread_id << "] " << std::regex_replace(funcName, std::regex("%"), "%%") << ": " << fmt << '\n';
  std::string final_fmt = ss.str();
  va_start(argptr, final_fmt.c_str());
  {
    //std::lock_guard<std::mutex> lock(logger::print_mutex);
    vfprintf(stderr, final_fmt.c_str(), argptr);
  }
  va_end(argptr);
}
#endif

#if LOGGER_LEVEL >= 2
void logWarning(const std::string& funcName, const std::string& fmt, ...) {
  if (logger_level < 2) {
    return;
  }

  va_list argptr;
  std::stringstream ss;
  ss << "[WARN ][" << logger::thread_id << "] " << std::regex_replace(funcName, std::regex("%"), "%%") << ": " << fmt << '\n';
  std::string final_fmt = ss.str();
  va_start(argptr, final_fmt.c_str());
  {
    //std::lock_guard<std::mutex> lock(logger::print_mutex);
    vfprintf(stderr, final_fmt.c_str(), argptr);
  }
  va_end(argptr);
}
#endif

#if LOGGER_LEVEL >= 3
void logInfo(const std::string& funcName, const std::string& fmt, ...) {
  if (logger_level < 3) {
    return;
  }

  va_list argptr;
  std::stringstream ss;
  ss << "[INFO ][" << logger::thread_id << "] " << std::regex_replace(funcName, std::regex("%"), "%%") << ": " << fmt << '\n';
  std::string final_fmt = ss.str();
  va_start(argptr, final_fmt.c_str());
  {
    //std::lock_guard<std::mutex> lock(logger::print_mutex);
    vfprintf(stdout, final_fmt.c_str(), argptr);
  }
  va_end(argptr);
}
#endif

#if LOGGER_LEVEL >= 4
void logDebug(const std::string& funcName, const std::string& fmt, ...) {
  if (logger_level < 4) {
    return;
  }

  va_list argptr;
  std::stringstream ss;
  ss << "[DEBUG][" << logger::thread_id << "] " << std::regex_replace(funcName, std::regex("%"), "%%") << ": " << fmt << '\n';
  std::string final_fmt = ss.str();
  va_start(argptr, final_fmt.c_str());
  {
    //std::lock_guard<std::mutex> lock(logger::print_mutex);
    vfprintf(stdout, final_fmt.c_str(), argptr);
  }
  va_end(argptr);
}
#endif

void setLoggerLevel(int level) {
  if (level < 0 || level > 4) {
    logErrorF("expected value from 0 to 4 [none, error, warning, info, debug]");
    return;
  }
  logger_level = level;
}
