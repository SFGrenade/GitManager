#include "ui/MainWindow.hpp"

// Project headers
#include <Log.hpp>

// Library headers
#include <wx/dirdlg.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/stattext.h>

// C++ headers
#include <array>

MainWindow::MainWindow() : wxFrame( nullptr, wxID_ANY, _( "Git Manager" ) ) {
  wxMenu* menuFile = new wxMenu;
  menuFile->Append( mwID_OpenFolder, _( "&Open Folder...\tCtrl+O" ) );
  menuFile->AppendSeparator();
  menuFile->Append( wxID_EXIT );

  wxMenu* menuHelp = new wxMenu;
  menuHelp->Append( wxID_ABOUT );

  wxMenuBar* menuBar = new wxMenuBar;
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
  wxStaticText* changedFileDisplayLabel = new wxStaticText( rightLeftPanel, wxID_ANY, "Changed Files Display" );
  wxStaticText* diffDisplayLabel = new wxStaticText( rightRightPanel, wxID_ANY, "Diff Display" );

  repoTreeList_ = new wxDataViewCtrl( leftPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_SINGLE | wxDV_ROW_LINES | wxDV_NO_HEADER );
  repoModel_ = decltype( repoModel_ )( new RepositoryModel() );

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

  rightLeftPanelSizer->Add( changedFileDisplayLabel, 1, wxEXPAND | wxALL );
  rightLeftPanel->SetSizer( rightLeftPanelSizer );

  rightRightPanelSizer->Add( diffDisplayLabel, 1, wxEXPAND | wxALL );
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
  repoTreeList_->AppendTextColumn( "Folder", 0 );

  Bind( wxEVT_MENU, &MainWindow::OnMenuOpenFolder, this, mwID_OpenFolder );
  Bind( wxEVT_MENU, &MainWindow::OnMenuAbout, this, wxID_ABOUT );
  Bind( wxEVT_MENU, &MainWindow::OnMenuExit, this, wxID_EXIT );
  Bind( wxEVT_THREAD, &MainWindow::OnThreadUpdate, this );

  UpdateUiWithNewRepos();
}

wxThread::ExitCode MainWindow::Entry() {
  repoModel_->SetBasePath( currentBasePath_ );
  wxQueueEvent( GetEventHandler(), new wxThreadEvent() );
  return wxThread::ExitCode( 0 );
}

void MainWindow::UpdateUiWithNewRepos() {
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
  wxMessageBox( "This is a wxWidgets Hello World example", "About Hello World", wxOK | wxICON_INFORMATION );
}

void MainWindow::OnMenuExit( wxCommandEvent& event ) {
  Close( true );
}

void MainWindow::OnThreadUpdate( wxThreadEvent& event ) {
  repoModel_->SetBasePath( currentBasePath_ );

  UpdateUiWithNewRepos();
}
