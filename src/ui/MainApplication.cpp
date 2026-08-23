#include "ui/MainApplication.hpp"

// Project headers
#include <ui/MainWindow.hpp>

bool MainApplication::OnInit() {
  // this is now owned by wxWidgets, it's not dangling
  return (new MainWindow())->Show();
}
