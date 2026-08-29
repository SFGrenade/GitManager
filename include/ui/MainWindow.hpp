#pragma once

// Project headers
#include <models/DiffModel.hpp>
#include <models/RepositoryModel.hpp>

// Library headers
#include <wx/dataview.h>
#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/thread.h>

// C++ headers
#include <filesystem>

class MainWindow : public wxFrame, public wxThreadHelper {
  public:
  enum MenuCommands { mwID_OpenFolder, mwID_Repo_Fetch, mwID_Repo_Pull, mwID_Repo_Commit, mwID_Repo_Push, mwID_Repo_CreateBranch, mwID_Repo_CheckoutBranch, mwID_Repo_DeleteBranch, mwID_Diff_Track, mwID_Diff_Untrack, mwID_Diff_Revert, mwID_Diff_Stage, mwID_Diff_Unstage };

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
  void OnRepoListSelectionChanged( wxDataViewEvent& event );
  void OnRepoListItemContextMenu( wxDataViewEvent& event );
  void OnPopupRepoFetch( wxCommandEvent& event );
  void OnPopupRepoPull( wxCommandEvent& event );
  void OnPopupRepoCommit( wxCommandEvent& event );
  void OnPopupRepoPush( wxCommandEvent& event );
  void OnPopupRepoCreateBranch( wxCommandEvent& event );
  void OnPopupRepoCheckoutBranch( wxCommandEvent& event );
  void OnPopupRepoDeleteBranch( wxCommandEvent& event );
  void OnDiffListSelectionChanged( wxDataViewEvent& event );
  void OnDiffListItemContextMenu( wxDataViewEvent& event );
  void OnPopupDiffTrack( wxCommandEvent& event );
  void OnPopupDiffUntrack( wxCommandEvent& event );
  void OnPopupDiffRevert( wxCommandEvent& event );
  void OnPopupDiffStage( wxCommandEvent& event );
  void OnPopupDiffUnstage( wxCommandEvent& event );

  protected:
  static int GitDiffLineCallback( git_diff_delta const* delta, git_diff_hunk const* hunk, git_diff_line const* line, void* user );
  void PrintGitError( std::string const& type, int errorCode ) const;

  private:
  wxDataViewCtrl* repoTreeList_ = nullptr;
  wxObjectDataPtr< RepositoryModel > repoModel_{};
  wxMenu* singleRepoPopupMenu_ = nullptr;

  wxDataViewCtrl* diffList_ = nullptr;
  wxObjectDataPtr< DiffModel > diffModel_{};
  wxMenu* singleDiffPopupMenu_ = nullptr;

  wxTextCtrl* diffDisplay_ = nullptr;
  std::filesystem::path diffDisplayFile_{};

  std::filesystem::path currentBasePath_{};
};
