#pragma once

// Project headers
#include <models/RepositoryModel.hpp>

// Library headers
#include <git2.h>
#include <wx/dataview.h>
#include <wx/frame.h>
#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/treelist.h>

// C++ headers
#include <filesystem>
#include <vector>

class MainWindow : public wxFrame {
  public:
  enum MenuCommands { mwID_OpenFolder };

  public:
  MainWindow();

  private:
  void UpdateUiWithNewRepos();
  void OnOpenFolder( wxCommandEvent& event );
  void OnAbout( wxCommandEvent& event );
  void OnExit( wxCommandEvent& event );

  private:
  wxPanel* panel_ = nullptr;
  wxSplitterWindow* splitterWindow_ = nullptr;
  wxPanel* leftPanel_ = nullptr;
  wxPanel* rightPanel_ = nullptr;
  // wxTreeListCtrl* repoTreeList_ = nullptr;
  wxDataViewCtrl* repoTreeList_ = nullptr;
  wxObjectDataPtr< RepositoryModel > repoModel_{};
  std::filesystem::path currentBasePath_{};
  std::vector< git_repository* > repos_{};
};
