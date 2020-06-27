#include <iostream>
#include <cstring>
#include <set>
#include <vector>
#include <core/Core.h>
#include "luaapi/LuaStateWrapper.h"

const char* HELP_STRING =
    R"(Usage: lucimalir [options] [filename1 filename2 ...]
If no filename is present, it will open an interactive shell.
Options:
 -h: display help
 -i: open interactive mode after processing files
)";

bool process_options(const char* arg, std::set<char>& options) {
  if (arg[0] != '-') {
    return false;
  }
  int len = strlen(arg);
  for (int i = 1; i < len; ++i) {
    options.insert(arg[i]);
  }
  return true;
}

#define LUCIMALIR_VERSION "0.1"

int main(int argc, const char** argv) {
  // fixes console output on CLion in debug configurations
#ifdef CLION_OUTPUT_DEBUG_FIX
  setbuf(stdout, nullptr);
  setbuf(stderr, nullptr);
#endif

  try {
    LuaStateWrapper lua_state(&std::cout, &std::cin, &std::cerr);
    std::vector<std::string> filenames;
    std::set<char> options;
    for (int i = 1; i < argc; ++i) {
      if (!process_options(argv[i], options)) {
        filenames.emplace_back(argv[i]);
      }
    }
    bool error = false;
    for (auto& f : filenames) {
      try {
        lua_state.doFile(f.c_str());
      } catch (std::exception& e) {
        std::cerr << "Error while processing " << f << "\n" << e.what();
        error = true;
        break;
      }
    }
    if (options.count('h')) {
      std::cout << HELP_STRING;
      return 0;
    }
    if ((filenames.empty() || options.count('i')) && !error) {
      std::string input;
      std::cout << "LUa Complex Interactive Mandelbrot-LIke fractal Renderer v"
                << LUCIMALIR_VERSION
                << "\nType \"help\" for help, \"exit\" to exit the program, \"reset\" to reset the lua state\n";
      while (std::cin.good()) {
        std::cout << "> ";
        std::cout.flush();
        std::getline(std::cin, input);
        if (input == "exit") {
          break;
        } else if (input == "reset") {
          lua_state.reset();
        } else {
          lua_state.doString(input);
        }
      }
    }
    auto& core = Core::getInstance();
    core.awaitCompletion();
    core.shutDown();
    return error ? 1 : 0;
  } catch (std::exception& e) {
    std::cerr << "Fatal exception:\n" << e.what();
    return 1;
  }
}
