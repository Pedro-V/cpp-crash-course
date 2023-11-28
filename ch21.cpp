// ---------------------------------------------------------------------------------
// Writing applications.

// Link with -lboost_program_options

#include <iostream>
#include <string>

// ---------------------------------------------------------------------------------
// Program support

// Program termination and cleanup

#include <cstdlib>

struct Tracer {
  Tracer(std::string name_in)
    : name{ std::move(name_in) } {
    std::cout << name << " constructed.\n";
  }

  ~Tracer() {
    std::cout << name << " destructed.\n";
  }

private:
  const std::string name;
};

// Tracer static_tracer{ "static Tracer" };

void run() {
  std::cout << "Entering run()\n";
  std::cout << "Registering a callback\n";
  // Gets invoked after main exits and all non-static objects have died.
  std::atexit([] { std::cout << "***std::atexit callback executing***\n"; });
  std::cout << "Callback registered\n";

  // Exiting doesn't return from main, so stack and dynamic objects are never
  // destructed! So it shouldn't be used.
  //std::exit(0);

  // Aborting doesn't call destructors nor atexit callbacks, returning directly
  // to the operating system.
  //std::abort();

  std::cout << "Exiting run()\n";
}

void termination_and_cleanup() {
  std::cout << "Entering main()\n";
  Tracer local_tracer{ "local Tracer" };
  thread_local Tracer thread_local_tracer{ "thread_local Tracer" };
  const auto* dynamic_tracer = new Tracer{ "dynamic Tracer" };
  run();
  delete dynamic_tracer;
  std::cout << "Exiting main()\n";
}

// Communicating with the environment

#include <cstdlib>

void communicating_with_env() {
  // Launching separate processes.
  std::string command{ "ping -c 4 google.com" };
  const auto result = std::system(command.c_str());
  std::cout << "The command \'" << command
            << "\' returned " << result << "\n";
  // Finding environment variables.
  std::string variable_name{ "PATH" };
  std::string var_env{ std::getenv(variable_name.c_str()) };
  std::cout << "The variable " << variable_name
            << " equals " << var_env << "\n";
}

// Managing OS signals

#include <csignal>
#include <chrono>
#include <thread>
#include <atomic>

std::atomic_bool interrupted{};

extern "C" void handler(int signal) {
  std::cout << "Handler invoked with signal " << signal << ".\n";
  interrupted = true;
}

void signals_handler() {
  using namespace std::chrono_literals;
  std::signal(SIGINT, handler);
  while(!interrupted) {
    std::cout << "Waiting..." << std::endl;
    std::this_thread::sleep_for(1s);
  }
  std::cout << "Interrupted!\n";
}

// ---------------------------------------------------------------------------------
// Boost ProgramOptions: a library for writing console applications.


#include <boost/program_options.hpp>

int mgrep(int argc, char** argv) {
  using namespace boost::program_options;
  bool is_recursive{}, is_help{};

  // Options description: specify the allowed options.
  options_description description{ "mgrep [options] pattern path1 path2 ..." };
  description.add_options()
    ("help,h", bool_switch(&is_help), "display a help dialog")
    ("threads,t", value<int>()->default_value(4), "number of threads to use")
    ("recursive,r", bool_switch(&is_recursive), "search subdirectories recusively")
    ("pattern", value<std::string>(), "pattern to search for")
    ("path", value<std::vector<std::string>>(), "path to search");

  positional_options_description positional;
  positional.add("pattern", 1);
  positional.add("path", -1);

  // Parsing options
  command_line_parser parser{ argc, argv };
  parser.options(description);
  parser.positional(positional);

  // Storing and accessing options
  variables_map vm;
  try {
    auto parsed_result = parser.run();
    store(parsed_result, vm);
    notify(vm);
  } catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
    return -1;
  }

  if (is_help) {
    std::cout << description;
    return 0;
  }
  if (vm["pattern"].empty()) {
    std::cerr << "You must provide a pattern.\n";
    return -1;
  } 
  if (vm["path"].empty()) {
    std::cerr << "You must provide at least one path.\n";
    return -1;
  }

  const auto threads = vm["threads"].as<int>();
  const auto& pattern = vm["pattern"].as<std::string>();
  const auto& paths = vm["path"].as<std::vector<std::string>>();
  
  // Implementation goes here.

  std::cout << "Ok." << std::endl;
  return 0;
}

// ---------------------------------------------------------------------------------

int main(int argc, char** argv) {
}
