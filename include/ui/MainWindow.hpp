#pragma once

// Project headers
#include <models/RepositoryModel.hpp>

// Library headers
#include <wx/dataview.h>
#include <wx/frame.h>
#include <wx/thread.h>

// C++ headers
#include <filesystem>

class MainWindow : public wxFrame, public wxThreadHelper {
  public:
  enum MenuCommands { mwID_OpenFolder };

  public:
  MainWindow();

  protected:
  virtual wxThread::ExitCode Entry() override;

  private:
  void UpdateUiWithNewRepos();
  void OnMenuOpenFolder( wxCommandEvent& event );
  void OnMenuAbout( wxCommandEvent& event );
  void OnMenuExit( wxCommandEvent& event );
  void OnThreadUpdate( wxThreadEvent& event );

  private:
  wxDataViewCtrl* repoTreeList_ = nullptr;
  wxObjectDataPtr< RepositoryModel > repoModel_{};
  std::filesystem::path currentBasePath_{};
};
