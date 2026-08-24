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

void RepositoryModel::GetValue( wxVariant& out_variant, wxDataViewItem const& item, uint32_t col ) const {
  Log::Trace( "RepositoryModel::GetValue(out_variant, item: %p, col: %d)", item.m_pItem, col );
  if( col == 0 ) {
    out_variant.SetName( "Folder Name" );
    out_variant = reinterpret_cast< InternalData* >( item.GetID() )->folderPath.filename().string();
  } else if( col == 1 ) {
    out_variant.SetName( "Path" );
    out_variant = reinterpret_cast< InternalData* >( item.GetID() )->folderPath.string();
  }
}

bool RepositoryModel::HasValue( wxDataViewItem const& item, uint32_t col ) const {
  Log::Trace( "RepositoryModel::HasValue(item: %p, col: %d)", item.m_pItem, col );
  if( col < 2 ) {
    return true;
  }
  if( !IsContainer( item ) ) {
    return true;
  }
  if( HasContainerColumns( item ) ) {
    return true;
  }
  return false;
}

bool RepositoryModel::SetValue( wxVariant const& variant, wxDataViewItem const& item, uint32_t col ) {
  Log::Trace( "RepositoryModel::SetValue(variant: {'%s', '%s'}, item: %p, col: %d)", variant.GetName().c_str().AsChar(), variant.GetData()->GetType().c_str().AsChar(), item.m_pItem, col );
  return false;
}

bool RepositoryModel::GetAttr( wxDataViewItem const& item, uint32_t col, wxDataViewItemAttr& out_attr ) const {
  Log::Trace( "RepositoryModel::GetAttr(item: %p, col: %d, out_attr)", item.m_pItem, col );
  return false;
}

bool RepositoryModel::IsEnabled( wxDataViewItem const& item, uint32_t col ) const {
  Log::Trace( "RepositoryModel::IsEnabled(item: %p, col: %d)", item.m_pItem, col );
  return true;
}

wxDataViewItem RepositoryModel::GetParent( wxDataViewItem const& item ) const {
  Log::Trace( "RepositoryModel::GetParent(item: %p)", item.m_pItem );
  return wxDataViewItem();
}

bool RepositoryModel::IsContainer( wxDataViewItem const& item ) const {
  Log::Trace( "RepositoryModel::IsContainer(item: %p)", item.m_pItem );
  return item.GetID() == nullptr;
}

bool RepositoryModel::HasContainerColumns( wxDataViewItem const& item ) const {
  Log::Trace( "RepositoryModel::HasContainerColumns(item: %p)", item.m_pItem );
  return false;
}

uint32_t RepositoryModel::GetChildren( wxDataViewItem const& item, wxDataViewItemArray& out_children ) const {
  Log::Trace( "RepositoryModel::GetChildren(item: %p, out_children)", item.m_pItem );
  if( item.GetID() == nullptr ) {
    for( auto const& item : items_ ) {
      out_children.Add( item );
    }
    return items_.size();
  } else {
    return 0;
  }
}

void RepositoryModel::Resort() {
  Log::Trace( "RepositoryModel::Resort()" );
}

int32_t RepositoryModel::Compare( wxDataViewItem const& item1, wxDataViewItem const& item2, uint32_t column, bool ascending ) const {
  Log::Trace( "RepositoryModel::Compare(item1: %p, item2: %p, column: %d, ascending: %d)", item1.m_pItem, item2.m_pItem, column, ascending );
  return int32_t{};
}

bool RepositoryModel::HasDefaultCompare() const {
  Log::Trace( "RepositoryModel::HasDefaultCompare()" );
  return true;
}

bool RepositoryModel::IsListModel() const {
  Log::Trace( "RepositoryModel::IsListModel()" );
  return false;
}

bool RepositoryModel::IsVirtualListModel() const {
  Log::Trace( "RepositoryModel::IsVirtualListModel()" );
  return false;
}
