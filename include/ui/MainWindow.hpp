#pragma once

// Project headers
#include <models/RepositoryModel.hpp>

// Library headers
#include <wx/dataview.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/splitter.h>

// C++ headers
#include <filesystem>

class MainWindow : public wxFrame {
  public:
  enum MenuCommands { mwID_OpenFolder };

  public:
  MainWindow();

  private:
  void UpdateUiWithNewRepos();
  void OnMenuOpenFolder( wxCommandEvent& event );
  void OnMenuAbout( wxCommandEvent& event );
  void OnMenuExit( wxCommandEvent& event );

  private:
  wxPanel* panel_ = nullptr;
  wxSplitterWindow* splitterWindow_ = nullptr;
  wxPanel* leftPanel_ = nullptr;
  wxPanel* rightPanel_ = nullptr;
  wxDataViewCtrl* repoTreeList_ = nullptr;
  wxObjectDataPtr< RepositoryModel > repoModel_{};
  std::filesystem::path currentBasePath_{};
};
