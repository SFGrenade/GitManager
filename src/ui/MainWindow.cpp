#include "ui/MainWindow.hpp"

// Project headers
#include <Log.hpp>

// Library headers
#include <wx/dirdlg.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/stattext.h>

// C++ headers
#include <array>

MainWindow::MainWindow() : wxFrame( nullptr, wxID_ANY, _( "Git Manager" ) ) {
  Log::Trace( "MainWindow::MainWindow()" );

  wxMenu* menuFile = new wxMenu();
  menuFile->Append( mwID_OpenFolder, _( "&Open Folder...\tCtrl+O" ) );
  menuFile->AppendSeparator();
  menuFile->Append( wxID_EXIT );

  wxMenu* menuHelp = new wxMenu();
  menuHelp->Append( wxID_ABOUT );

  wxMenuBar* menuBar = new wxMenuBar();
  menuBar->Append( menuFile, _( "&File" ) );
  menuBar->Append( menuHelp, _( "&Help" ) );

  SetMenuBar( menuBar );

  CreateStatusBar();

  wxPanel* panel = new wxPanel( this );
  wxSplitterWindow* splitterWindow1 = new wxSplitterWindow( panel );
  wxPanel* leftPanel = new wxPanel( splitterWindow1 );
  wxPanel* rightPanel = new wxPanel( splitterWindow1 );
  wxSplitterWindow* splitterWindow2 = new wxSplitterWindow( rightPanel );
  wxPanel* rightLeftPanel = new wxPanel( splitterWindow2 );
  wxPanel* rightRightPanel = new wxPanel( splitterWindow2 );

  repoTreeList_ = new wxDataViewCtrl( leftPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_SINGLE | wxDV_ROW_LINES /*| wxDV_NO_HEADER*/ );
  repoModel_ = decltype( repoModel_ )( new RepositoryModel() );

  diffList_ = new wxDataViewCtrl( rightLeftPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_SINGLE | wxDV_ROW_LINES /*| wxDV_NO_HEADER*/ );
  diffModel_ = decltype( diffModel_ )( new DiffModel() );

  diffDisplay_ = new wxTextCtrl( rightRightPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2 | wxTE_NOHIDESEL | wxHSCROLL | wxTE_LEFT | wxTE_DONTWRAP );

  wxGridSizer* panelSizer = new wxGridSizer( 1, 1, 0, 0 );
  wxGridSizer* leftPanelSizer = new wxGridSizer( 1, 1, 0, 0 );
  wxGridSizer* rightPanelSizer = new wxGridSizer( 1, 1, 0, 0 );
  wxGridSizer* rightLeftPanelSizer = new wxGridSizer( 1, 1, 0, 0 );
  wxGridSizer* rightRightPanelSizer = new wxGridSizer( 1, 1, 0, 0 );

  panelSizer->Add( splitterWindow1, 1, wxEXPAND | wxALL );
  panel->SetSizer( panelSizer );

  leftPanelSizer->Add( repoTreeList_, 1, wxEXPAND | wxALL );
  leftPanel->SetSizer( leftPanelSizer );

  rightPanelSizer->Add( splitterWindow2, 1, wxEXPAND | wxALL );
  rightPanel->SetSizer( rightPanelSizer );

  rightLeftPanelSizer->Add( diffList_, 1, wxEXPAND | wxALL );
  rightLeftPanel->SetSizer( rightLeftPanelSizer );

  rightRightPanelSizer->Add( diffDisplay_, 1, wxEXPAND | wxALL );
  rightRightPanel->SetSizer( rightRightPanelSizer );

  splitterWindow1->SetSplitMode( wxSPLIT_VERTICAL );
  splitterWindow1->SplitVertically( leftPanel, rightPanel );
  splitterWindow1->SetSashGravity( 1.0 / 3.0 );
  splitterWindow1->SetMinimumPaneSize( 10 );

  splitterWindow2->SetSplitMode( wxSPLIT_VERTICAL );
  splitterWindow2->SplitVertically( rightLeftPanel, rightRightPanel );
  splitterWindow2->SetSashGravity( 1.0 / 2.0 );
  splitterWindow2->SetMinimumPaneSize( 10 );

  repoTreeList_->AssociateModel( repoModel_.get() );
  repoTreeList_->AppendTextColumn( "FolderName", RepositoryModel::Columns::FolderName );
  // repoTreeList_->AppendTextColumn( "Depth", RepositoryModel::Columns::Depth );
  // repoTreeList_->AppendTextColumn( "Path", RepositoryModel::Columns::Path );
  repoTreeList_->AppendTextColumn( "CurrentBranch", RepositoryModel::Columns::CurrentBranch );
  repoTreeList_->AppendTextColumn( "CurrentStatus", RepositoryModel::Columns::CurrentStatus );

  diffList_->AssociateModel( diffModel_.get() );
  diffList_->AppendTextColumn( "Path", DiffModel::Columns::Path );
  diffList_->AppendTextColumn( "Status", DiffModel::Columns::Status );

  singleRepoPopupMenu_ = new wxMenu();
  singleRepoPopupMenu_->Append( mwID_Repo_Fetch, _( "Fetch" ) );
  singleRepoPopupMenu_->Append( mwID_Repo_Pull, _( "Pull" ) );
  singleRepoPopupMenu_->Append( mwID_Repo_Commit, _( "Commit..." ) );
  singleRepoPopupMenu_->Append( mwID_Repo_Push, _( "Push" ) );
  singleRepoPopupMenu_->AppendSeparator();
  singleRepoPopupMenu_->Append( mwID_Repo_CreateBranch, _( "Create Branch..." ) );
  singleRepoPopupMenu_->Append( mwID_Repo_CheckoutBranch, _( "Checkout Branch..." ) );
  singleRepoPopupMenu_->Append( mwID_Repo_DeleteBranch, _( "Delete Branch..." ) );

  singleDiffPopupMenu_ = new wxMenu();
  singleDiffPopupMenu_->Append( mwID_Diff_Track, _( "Track" ) );
  singleDiffPopupMenu_->Append( mwID_Diff_Untrack, _( "Untrack" ) );
  singleDiffPopupMenu_->Append( mwID_Diff_Revert, _( "Revert" ) );
  singleDiffPopupMenu_->Append( mwID_Diff_Stage, _( "Stage" ) );
  singleDiffPopupMenu_->Append( mwID_Diff_Unstage, _( "Unstage" ) );

  Bind( wxEVT_MENU, &MainWindow::OnMenuOpenFolder, this, mwID_OpenFolder );
  Bind( wxEVT_MENU, &MainWindow::OnMenuAbout, this, wxID_ABOUT );
  Bind( wxEVT_MENU, &MainWindow::OnMenuExit, this, wxID_EXIT );
  Bind( wxEVT_THREAD, &MainWindow::OnThreadUpdate, this );
  Bind( wxEVT_DATAVIEW_SELECTION_CHANGED, &MainWindow::OnRepoListSelectionChanged, this, repoTreeList_->GetId() );
  Bind( wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &MainWindow::OnRepoListItemContextMenu, this, repoTreeList_->GetId() );
  Bind( wxEVT_MENU, &MainWindow::OnPopupRepoFetch, this, mwID_Repo_Fetch );
  Bind( wxEVT_MENU, &MainWindow::OnPopupRepoPull, this, mwID_Repo_Pull );
  Bind( wxEVT_MENU, &MainWindow::OnPopupRepoCommit, this, mwID_Repo_Commit );
  Bind( wxEVT_MENU, &MainWindow::OnPopupRepoPush, this, mwID_Repo_Push );
  Bind( wxEVT_MENU, &MainWindow::OnPopupRepoCreateBranch, this, mwID_Repo_CreateBranch );
  Bind( wxEVT_MENU, &MainWindow::OnPopupRepoCheckoutBranch, this, mwID_Repo_CheckoutBranch );
  Bind( wxEVT_MENU, &MainWindow::OnPopupRepoDeleteBranch, this, mwID_Repo_DeleteBranch );
  Bind( wxEVT_DATAVIEW_SELECTION_CHANGED, &MainWindow::OnDiffListSelectionChanged, this, diffList_->GetId() );
  Bind( wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &MainWindow::OnDiffListItemContextMenu, this, diffList_->GetId() );
  Bind( wxEVT_MENU, &MainWindow::OnPopupDiffTrack, this, mwID_Diff_Track );
  Bind( wxEVT_MENU, &MainWindow::OnPopupDiffUntrack, this, mwID_Diff_Untrack );
  Bind( wxEVT_MENU, &MainWindow::OnPopupDiffRevert, this, mwID_Diff_Revert );
  Bind( wxEVT_MENU, &MainWindow::OnPopupDiffStage, this, mwID_Diff_Stage );
  Bind( wxEVT_MENU, &MainWindow::OnPopupDiffUnstage, this, mwID_Diff_Unstage );

  UpdateUiWithNewRepos();
}

wxThread::ExitCode MainWindow::Entry() {
  Log::Trace( "MainWindow::Entry()" );

  repoModel_->SetBasePath( currentBasePath_ );
  wxQueueEvent( GetEventHandler(), new wxThreadEvent() );
  return wxThread::ExitCode( 0 );
}

void MainWindow::UpdateUiWithNewRepos() {
  Log::Trace( "MainWindow::UpdateUiWithNewRepos()" );

  std::array< char, 256 > tmp;

  tmp.fill( '\0' );
  if( currentBasePath_ != std::filesystem::path{} ) {
    snprintf( tmp.data(), tmp.size(), "%s: %s", _( "Git Manager" ).c_str().AsChar(), currentBasePath_.filename().string().c_str() );
  } else {
    snprintf( tmp.data(), tmp.size(), "%s", _( "Git Manager" ).c_str().AsChar() );
  }
  SetTitle( tmp.data() );

  tmp.fill( '\0' );
  {
    wxDataViewItem tmpParent;
    wxDataViewItemArray tmpChildren;
    snprintf( tmp.data(), tmp.size(), "%d %s", repoModel_->GetChildren( tmpParent, tmpChildren ), _( "Repositories scanned" ).c_str().AsChar() );
  }
  SetStatusText( tmp.data() );
}

void MainWindow::OnMenuOpenFolder( wxCommandEvent& event ) {
  Log::Trace( "MainWindow::OnMenuOpenFolder( event )" );

  wxDirDialog openFolderDialog( this );
  if( openFolderDialog.ShowModal() == wxID_CANCEL ) {
    return;
  }
  currentBasePath_ = openFolderDialog.GetPath().utf8_string();

  if( CreateThread( wxTHREAD_JOINABLE ) != wxTHREAD_NO_ERROR ) {
    Log::Error( "Could not create the worker thread!" );
    return;
  }

  if( GetThread()->Run() != wxTHREAD_NO_ERROR ) {
    Log::Error( "Could not run the worker thread!" );
    return;
  }
}

void MainWindow::OnMenuAbout( wxCommandEvent& event ) {
  Log::Trace( "MainWindow::OnMenuAbout( event )" );

  wxMessageBox( "This is a wxWidgets Hello World example", "About Hello World", wxOK | wxICON_INFORMATION );
}

void MainWindow::OnMenuExit( wxCommandEvent& event ) {
  Log::Trace( "MainWindow::OnMenuExit( event )" );

  Close( true );
}

void MainWindow::OnThreadUpdate( wxThreadEvent& event ) {
  Log::Trace( "MainWindow::OnThreadUpdate( event )" );

  repoModel_->SetBasePath( currentBasePath_ );

  UpdateUiWithNewRepos();
}

void MainWindow::OnRepoListSelectionChanged( wxDataViewEvent& event ) {
  Log::Trace( "MainWindow::OnRepoListSelectionChanged( event )" );

  wxDataViewItem tmpItem = event.GetItem();
  if( !tmpItem.IsOk() ) {
    diffModel_->SetRepository( nullptr );
    diffDisplay_->Clear();
    return;
  }

  wxDataViewItem& tmpItem2 = tmpItem;
  RepositoryModel::Data& item = reinterpret_cast< RepositoryModel::Data& >( tmpItem2 );

  diffModel_->SetRepository( item.GetID() );
  diffDisplay_->Clear();
}

void MainWindow::OnRepoListItemContextMenu( wxDataViewEvent& event ) {
  Log::Trace( "MainWindow::OnRepoListItemContextMenu( event )" );

  wxDataViewItem tmpItem = event.GetItem();
  if( !tmpItem.IsOk() ) {
    return;
  }

  wxDataViewItem& tmpItem2 = tmpItem;
  RepositoryModel::Data& item = reinterpret_cast< RepositoryModel::Data& >( tmpItem2 );

  PopupMenu( singleRepoPopupMenu_ );
}

void MainWindow::OnPopupRepoFetch( wxCommandEvent& event ) {
  Log::Trace( "MainWindow::OnPopupRepoFetch( event )" );
}

void MainWindow::OnPopupRepoPull( wxCommandEvent& event ) {
  Log::Trace( "MainWindow::OnPopupRepoPull( event )" );
}

void MainWindow::OnPopupRepoCommit( wxCommandEvent& event ) {
  Log::Trace( "MainWindow::OnPopupRepoCommit( event )" );
}

void MainWindow::OnPopupRepoPush( wxCommandEvent& event ) {
  Log::Trace( "MainWindow::OnPopupRepoPush( event )" );
}

void MainWindow::OnPopupRepoCreateBranch( wxCommandEvent& event ) {
  Log::Trace( "MainWindow::OnPopupRepoCreateBranch( event )" );
}

void MainWindow::OnPopupRepoCheckoutBranch( wxCommandEvent& event ) {
  Log::Trace( "MainWindow::OnPopupRepoCheckoutBranch( event )" );
}

void MainWindow::OnPopupRepoDeleteBranch( wxCommandEvent& event ) {
  Log::Trace( "MainWindow::OnPopupRepoDeleteBranch( event )" );
}

void MainWindow::OnDiffListSelectionChanged( wxDataViewEvent& event ) {
  Log::Trace( "MainWindow::OnDiffListSelectionChanged( event )" );

  wxDataViewItem tmpItem = event.GetItem();
  if( !tmpItem.IsOk() ) {
    diffDisplay_->Clear();
    return;
  }

  wxDataViewItem& tmpItem2 = tmpItem;
  DiffModel::Data& item = reinterpret_cast< DiffModel::Data& >( tmpItem2 );

  diffDisplay_->Clear();
  diffDisplayFile_ = item->GetPath();

  if( int error = git_diff_foreach( diffModel_->GetDiff().get(), nullptr, nullptr, nullptr, GitDiffLineCallback, this ); error < 0 ) {
    PrintGitError( "git_diff_foreach", error );
    return;
  }
}

void MainWindow::OnDiffListItemContextMenu( wxDataViewEvent& event ) {
  Log::Trace( "MainWindow::OnDiffListItemContextMenu( event )" );

  wxDataViewItem tmpItem = event.GetItem();
  if( !tmpItem.IsOk() ) {
    return;
  }

  wxDataViewItem& tmpItem2 = tmpItem;
  DiffModel::Data& item = reinterpret_cast< DiffModel::Data& >( tmpItem2 );

  PopupMenu( singleDiffPopupMenu_ );
}

void MainWindow::OnPopupDiffTrack( wxCommandEvent& event ) {
  Log::Trace( "MainWindow::OnPopupDiffTrack( event )" );
}

void MainWindow::OnPopupDiffUntrack( wxCommandEvent& event ) {
  Log::Trace( "MainWindow::OnPopupDiffUntrack( event )" );
}

void MainWindow::OnPopupDiffRevert( wxCommandEvent& event ) {
  Log::Trace( "MainWindow::OnPopupDiffRevert( event )" );
}

void MainWindow::OnPopupDiffStage( wxCommandEvent& event ) {
  Log::Trace( "MainWindow::OnPopupDiffStage( event )" );
}

void MainWindow::OnPopupDiffUnstage( wxCommandEvent& event ) {
  Log::Trace( "MainWindow::OnPopupDiffUnstage( event )" );
}

int MainWindow::GitDiffLineCallback( git_diff_delta const* delta, git_diff_hunk const* hunk, git_diff_line const* line, void* user ) {
  Log::Trace( "MainWindow::GitDiffLineCallback( delta: %p, hunk: %p, line: %p, user: %p )", delta, hunk, line, user );

  MainWindow* self = reinterpret_cast< MainWindow* >( user );

  std::filesystem::path deltaNewFilePath( delta->new_file.path );
  if( self->diffDisplayFile_ != deltaNewFilePath ) {
    return 0;
  }

  auto defaultStyle = self->diffDisplay_->GetDefaultStyle();
  wxColour defaultTextColour = defaultStyle.GetTextColour();
  wxColour defaultBackgroundColour = defaultStyle.GetBackgroundColour();

  if( line->origin == GIT_DIFF_LINE_CONTEXT ) {
    // nothing special for context
  } else if( line->origin == GIT_DIFF_LINE_ADDITION ) {
    self->diffDisplay_->SetDefaultStyle( wxTextAttr( wxNullColour, *wxGREEN ) );
  } else if( line->origin == GIT_DIFF_LINE_DELETION ) {
    self->diffDisplay_->SetDefaultStyle( wxTextAttr( wxNullColour, *wxRED ) );
  }

  self->diffDisplay_->AppendText( wxString( line->content, line->content_len ) );

  self->diffDisplay_->SetDefaultStyle( wxTextAttr( defaultTextColour, defaultBackgroundColour ) );

  return 0;
}

void MainWindow::PrintGitError( std::string const& type, int errorCode ) const {
  git_error const* e = git_error_last();
  Log::Error( "%s error: %d/%d: %s", type.c_str(), errorCode, e->klass, e->message );
}
