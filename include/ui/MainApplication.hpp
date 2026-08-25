#pragma once

// Library headers
#include <wx/app.h>

class MainApplication : public wxApp {
  public:
  bool OnInit() override;
  int OnExit() override;
};

// This defines the equivalent of main() for the current platform.
wxIMPLEMENT_APP( MainApplication );
