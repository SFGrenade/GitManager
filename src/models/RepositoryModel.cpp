#include "models/RepositoryModel.hpp"

// Project headers
#include <Log.hpp>

// Library headers
#include <wx/log.h>

// C++ headers
#include <queue>

RepositoryModel::RepositoryModel() {
  Log::Trace( "RepositoryModel::RepositoryModel()" );
}

void RepositoryModel::SetBasePath( std::filesystem::path const& basePath ) {
  basePath_ = basePath;

  ItemsDeleted( wxDataViewItem(), wxDataViewItemArray( items_.begin(), items_.end() ) );
  items_.clear();

  std::queue< std::filesystem::path > pathsToCheckForRepos;
  pathsToCheckForRepos.push( basePath_ );
  while( !pathsToCheckForRepos.empty() ) {
    std::filesystem::path pathToCheck = pathsToCheckForRepos.front();
    pathsToCheckForRepos.pop();

    git_repository* repo = nullptr;
    int error = git_repository_open( &repo, pathToCheck.string().c_str() );
    if( error == 0 ) {
      // no error
      items_.push_back( Data( new InternalData{ .folderPath = pathToCheck, .gitRepo = std::shared_ptr< git_repository >( repo, []( git_repository* p ) { git_repository_free( p ); } ) } ) );
    } else if( error < 0 ) {
      // error
      git_error const* e = git_error_last();
      Log::Error( "Git error at '%s': %d/%d: %s", pathToCheck.string().c_str(), error, e->klass, e->message );
    }

    for( std::filesystem::directory_entry const& dirEntry : std::filesystem::directory_iterator( pathToCheck ) ) {
      if( !dirEntry.is_directory() ) {
        continue;
      }
      if( dirEntry.path().filename() == ".git" ) {
        continue;
      }
      pathsToCheckForRepos.push( dirEntry.path() );
    }
  }
  ItemsAdded( wxDataViewItem(), wxDataViewItemArray( items_.begin(), items_.end() ) );
}

void RepositoryModel::GetValue( wxVariant& variant, wxDataViewItem const& item, uint32_t col ) const {
  Log::Trace( "RepositoryModel::GetValue()" );
}

bool RepositoryModel::HasValue( wxDataViewItem const& item, uint32_t col ) const {
  Log::Trace( "RepositoryModel::HasValue()" );
  return bool{};
}

bool RepositoryModel::SetValue( wxVariant const& variant, wxDataViewItem const& item, uint32_t col ) {
  Log::Trace( "RepositoryModel::SetValue()" );
  return bool{};
}

bool RepositoryModel::GetAttr( wxDataViewItem const& item, uint32_t col, wxDataViewItemAttr& out_attr ) const {
  Log::Trace( "RepositoryModel::GetAttr()" );
  return bool{};
}

bool RepositoryModel::IsEnabled( wxDataViewItem const& item, uint32_t col ) const {
  Log::Trace( "RepositoryModel::IsEnabled()" );
  return bool{};
}

wxDataViewItem RepositoryModel::GetParent( wxDataViewItem const& item ) const {
  Log::Trace( "RepositoryModel::GetParent()" );
  return wxDataViewItem{};
}

bool RepositoryModel::IsContainer( wxDataViewItem const& item ) const {
  Log::Trace( "RepositoryModel::IsContainer()" );
  return bool{};
}

bool RepositoryModel::HasContainerColumns( wxDataViewItem const& item ) const {
  Log::Trace( "RepositoryModel::HasContainerColumns()" );
  return bool{};
}

uint32_t RepositoryModel::GetChildren( wxDataViewItem const& item, wxDataViewItemArray& children ) const {
  Log::Trace( "RepositoryModel::GetChildren()" );
  return uint32_t{};
}

void RepositoryModel::Resort() {
  Log::Trace( "RepositoryModel::Resort()" );
}

int32_t RepositoryModel::Compare( wxDataViewItem const& item1, wxDataViewItem const& item2, uint32_t column, bool ascending ) const {
  Log::Trace( "RepositoryModel::Compare()" );
  return int32_t{};
}

bool RepositoryModel::HasDefaultCompare() const {
  Log::Trace( "RepositoryModel::HasDefaultCompare()" );
  return bool{};
}

bool RepositoryModel::IsListModel() const {
  Log::Trace( "RepositoryModel::IsListModel()" );
  return bool{};
}

bool RepositoryModel::IsVirtualListModel() const {
  Log::Trace( "RepositoryModel::IsVirtualListModel()" );
  return bool{};
}
