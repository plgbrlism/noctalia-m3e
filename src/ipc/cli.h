#pragma once

namespace vespertine::ipc {

  // Entry point for `vespertine msg <command> [args...]`. Returns a process exit
  // code. Forwards the command to the running instance over the IPC socket.
  int runCli(int argc, char* argv[]);

} // namespace vespertine::ipc
