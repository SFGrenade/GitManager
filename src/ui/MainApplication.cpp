#include "ui/MainApplication.hpp"

// Project headers
#include <Log.hpp>
#include <ui/MainWindow.hpp>

// Library headers
#include <git2.h>

bool MainApplication::OnInit() {
  // Set up Log
  Log::Init();

  git_libgit2_init();

  // this is now owned by wxWidgets, it's not dangling
  ( new MainWindow() )->Show();

  return true;
}

int MainApplication::OnExit() {
  git_libgit2_shutdown();

  // Clean up Log
  Log::DeInit();

  return 0;
}
