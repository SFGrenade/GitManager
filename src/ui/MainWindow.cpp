#include "ui/MainWindow.hpp"

// Library headers
#include <wx/dirdlg.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>

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

  panel_ = new wxPanel( this );
  splitterWindow_ = new wxSplitterWindow( panel_ );
  leftPanel_ = new wxPanel( splitterWindow_ );
  rightPanel_ = new wxPanel( splitterWindow_ );
  // repoTreeList_ = new wxTreeListCtrl( leftPanel_, wxID_ANY );
  repoTreeList_ = new wxDataViewCtrl( leftPanel_, wxID_ANY );
  repoModel_ = decltype( repoModel_ )( new RepositoryModel() );

  wxGridSizer* panelSizer = new wxGridSizer( 1, 1, 0, 0 );
  wxGridSizer* leftPanelSizer = new wxGridSizer( 1, 1, 0, 0 );
  wxGridSizer* rightPanelSizer = new wxGridSizer( 1, 1, 0, 0 );

  panelSizer->Add( splitterWindow_, 1, wxEXPAND | wxALL );
  panel_->SetSizer( panelSizer );

  leftPanelSizer->Add( repoTreeList_, 1, wxEXPAND | wxALL );
  leftPanel_->SetSizer( leftPanelSizer );

  // rightPanelSizer->Add( splitterWindow_, 1, wxEXPAND | wxALL );
  rightPanel_->SetSizer( rightPanelSizer );

  splitterWindow_->SetSplitMode( wxSPLIT_VERTICAL );
  splitterWindow_->SplitVertically( leftPanel_, rightPanel_ );
  splitterWindow_->SetSashGravity( 1.0 / 3.0 );

  repoTreeList_->AssociateModel( repoModel_.get() );
  repoTreeList_->AppendTextColumn( "Folder", 0 );
  repoTreeList_->AppendTextColumn( "Depth", 1 );
  repoTreeList_->AppendTextColumn( "Path", 2 );
  // repoTreeList_->AppendColumn( _( "Repositories" ) );

  Bind( wxEVT_MENU, &MainWindow::OnMenuOpenFolder, this, mwID_OpenFolder );
  Bind( wxEVT_MENU, &MainWindow::OnMenuAbout, this, wxID_ABOUT );
  Bind( wxEVT_MENU, &MainWindow::OnMenuExit, this, wxID_EXIT );

  UpdateUiWithNewRepos();
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

  repoModel_->SetBasePath( currentBasePath_ );

  UpdateUiWithNewRepos();
}

void MainWindow::OnMenuAbout( wxCommandEvent& event ) {
  wxMessageBox( "This is a wxWidgets Hello World example", "About Hello World", wxOK | wxICON_INFORMATION );
}

void MainWindow::OnMenuExit( wxCommandEvent& event ) {
  Close( true );
}
