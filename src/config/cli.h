#pragma once

namespace vespertine::config {

  // Entry point for `vespertine config <command> [options]`. Returns a process
  // exit code. Pure CLI helper; does not start Application or mutate live config.
  int runCli(int argc, char* argv[]);

} // namespace vespertine::config
