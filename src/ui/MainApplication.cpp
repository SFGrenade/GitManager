#include "ui/MainApplication.hpp"

// Project headers
#include <Log.hpp>
#include <ui/MainWindow.hpp>

bool MainApplication::OnInit() {
  // Set up Log
  Log::Init();

  // this is now owned by wxWidgets, it's not dangling
  return ( new MainWindow() )->Show();
}
