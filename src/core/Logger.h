//
// Created by sugoi on 17.01.19.
//

#ifndef LUCIMALIR_LOGGER_H
#define LUCIMALIR_LOGGER_H

#include <string>
#include <mutex>

namespace logger {
  extern std::mutex print_mutex;
  extern int highest_thread_id;
  extern thread_local int thread_id;

  void register_logger_thread_id();

  inline void function_that_does_nothing_but_doesnt_fuck_up_auto_formatting_unlike_semicolon() {}
};


std::string demangle(const char* mangledName);

#if LOGGER_LEVEL >= 1
void logError(const std::string& funcName, const std::string& fmt, ...);
#else
#define logError(...) logger::function_that_does_nothing_but_doesnt_fuck_up_auto_formatting_unlike_semicolon();
#endif

#if LOGGER_LEVEL >= 2
void logWarning(const std::string& funcName, const std::string& fmt, ...);
#else
#define logWarning(...) logger::function_that_does_nothing_but_doesnt_fuck_up_auto_formatting_unlike_semicolon();
#endif

#if LOGGER_LEVEL >= 3
void logInfo(const std::string& funcName, const std::string& fmt, ...);
#else
#define logInfo(...) logger::function_that_does_nothing_but_doesnt_fuck_up_auto_formatting_unlike_semicolon();
#endif

#if LOGGER_LEVEL >= 4
void logDebug(const std::string& funcName, const std::string& fmt, ...);
#else
#define logDebug(...) logger::function_that_does_nothing_but_doesnt_fuck_up_auto_formatting_unlike_semicolon();
#endif
//void logDebug(const std::string& fmt, ...);

#define CLASS_NAME(obj) demangle(typeid(obj).name())
#define THIS_CLASS_NAME CLASS_NAME(*this)
#define CLASS_AND_FUNCTION (THIS_CLASS_NAME + "::" + __FUNCTION__).c_str()

#define logErrorF(msg, ...) logError(__FUNCTION__, msg, ##__VA_ARGS__);
#define logWarningF(msg, ...) logWarning(__FUNCTION__, msg, ##__VA_ARGS__);
#define logInfoF(msg, ...) logInfo(__FUNCTION__, msg, ##__VA_ARGS__);
#define logDebugF(msg, ...) logDebug(__FUNCTION__, msg, ##__VA_ARGS__);

#define logErrorC(msg, ...) logError(CLASS_AND_FUNCTION, msg, ##__VA_ARGS__);
#define logWarningC(msg, ...) logWarning(CLASS_AND_FUNCTION, msg, ##__VA_ARGS__);
#define logInfoC(msg, ...) logInfo(CLASS_AND_FUNCTION, msg, ##__VA_ARGS__);
#define logDebugC(msg, ...) logDebug(CLASS_AND_FUNCTION, msg, ##__VA_ARGS__);

#ifdef HEAVY_DEBUG

#define heavyDebug(msg, ...) logDebug(msg, ##__VA_ARGS__);
#define heavyDebugF(msg, ...) logDebugF(msg, ##__VA_ARGS__);
#define heavyDebugC(msg, ...) logDebugC(msg, ##__VA_ARGS__);

#else

#define heavyDebug(msg, ...) logger::function_that_does_nothing_but_doesnt_fuck_up_auto_formatting_unlike_semicolon();
#define heavyDebugF(msg, ...) logger::function_that_does_nothing_but_doesnt_fuck_up_auto_formatting_unlike_semicolon();
#define heavyDebugC(msg, ...) logger::function_that_does_nothing_but_doesnt_fuck_up_auto_formatting_unlike_semicolon();

#endif


/* LUA-CALLABLE FUNCTIONS */
extern "C" {

void setLoggerLevel(int level);

}


#endif //LUCIMALIR_LOGGER_H
