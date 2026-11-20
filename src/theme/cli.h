#pragma once

namespace vespertine::theme {

  // Entry point for `vespertine theme <image> [options]`. Returns a process exit
  // code. Does not touch Application / event loop / config — pure function of
  // (argv, stdout, stderr).
  int runCli(int argc, char* argv[]);

} // namespace vespertine::theme
