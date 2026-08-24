#include "models/RepositoryModel.hpp"

// Project headers
#include <Log.hpp>
#include <utils.hpp>

// Library headers
#include <wx/log.h>

// C++ headers
#include <list>

RepositoryModel::RepositoryModel() {
  Log::Trace( "RepositoryModel::RepositoryModel()" );
}

void RepositoryModel::SetBasePath( std::filesystem::path const& basePath ) {
  basePath_ = basePath;

  ItemsDeleted( wxDataViewItem(), wxDataViewItemArray( items_.begin(), items_.end() ) );
  items_.clear();

  ScanPath( basePath_, 0 );
  for( auto iter = std::filesystem::recursive_directory_iterator( basePath_ ); iter != std::filesystem::recursive_directory_iterator(); iter++ ) {
    if( !std::filesystem::is_directory( *iter ) ) {
      continue;
    }
    if( iter->path().filename() == ".git" ) {
      // we don't care about git data folders
      continue;
    }
    if( iter->path().filename() == ".svn" ) {
      // we don't care about svn data folders
      continue;
    }
    if( iter->path().filename().string().starts_with( "." ) ) {
      // ignore hidden folders for now
      continue;
    }
    ScanPath( iter->path(), uint32_t( iter.depth() + 1 ) );
  }


  ItemsAdded( wxDataViewItem(), wxDataViewItemArray( items_.begin(), items_.end() ) );
}

void RepositoryModel::GetValue( wxVariant& out_variant, wxDataViewItem const& item, uint32_t col ) const {
  if( col == 0 ) {
    out_variant = wxVariant( reinterpret_cast< InternalData* >( item.GetID() )->folderPath.filename().string(), "Folder Name" );
  } else if( col == 1 ) {
    out_variant = wxVariant( std::to_string( reinterpret_cast< InternalData* >( item.GetID() )->depth ), "Depth" );
  } else if( col == 2 ) {
    out_variant = wxVariant( reinterpret_cast< InternalData* >( item.GetID() )->folderPath.string(), "Path" );
  }
}

bool RepositoryModel::HasValue( wxDataViewItem const& item, uint32_t col ) const {
  if( col < 3 ) {
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

bool RepositoryModel::SetValue( wxVariant const& /*variant*/, wxDataViewItem const& /*item*/, uint32_t /*col*/ ) {
  return false;
}

bool RepositoryModel::GetAttr( wxDataViewItem const& /*item*/, uint32_t /*col*/, wxDataViewItemAttr& /*out_attr*/ ) const {
  return false;
}

bool RepositoryModel::IsEnabled( wxDataViewItem const& /*item*/, uint32_t /*col*/ ) const {
  return true;
}

wxDataViewItem RepositoryModel::GetParent( wxDataViewItem const& /*item*/ ) const {
  return wxDataViewItem();
}

bool RepositoryModel::IsContainer( wxDataViewItem const& item ) const {
  return item.GetID() == nullptr;
}

bool RepositoryModel::HasContainerColumns( wxDataViewItem const& /*item*/ ) const {
  return false;
}

uint32_t RepositoryModel::GetChildren( wxDataViewItem const& item, wxDataViewItemArray& out_children ) const {
  if( item.GetID() == nullptr ) {
    out_children = wxDataViewItemArray( items_.begin(), items_.end() );
    return items_.size();
  } else {
    return 0;
  }
}

void RepositoryModel::Resort() {}

int32_t RepositoryModel::Compare( wxDataViewItem const& item1, wxDataViewItem const& item2, uint32_t column, bool ascending ) const {
  if( column == -1 ) {
    // default
    InternalData& tmp1 = *reinterpret_cast< InternalData* >( item1.GetID() );
    InternalData& tmp2 = *reinterpret_cast< InternalData* >( item2.GetID() );
    if( tmp1.depth < tmp2.depth ) {
      return ascending ? -1 : 1;
    } else if( tmp1.depth > tmp2.depth ) {
      return ascending ? 1 : -1;
    }
    std::string path1 = tmp1.folderPath.string();
    std::string path2 = tmp2.folderPath.string();
    toLower( path1 );
    toLower( path2 );
    int tmpRet = path1.compare( path2 );
    return ascending ? tmpRet : -tmpRet;
  } else if( column == 0 ) {
    std::string tmp1 = reinterpret_cast< InternalData* >( item1.GetID() )->folderPath.filename().string();
    std::string tmp2 = reinterpret_cast< InternalData* >( item2.GetID() )->folderPath.filename().string();
    toLower( tmp1 );
    toLower( tmp2 );
    int tmpRet = tmp1.compare( tmp2 );
    return ascending ? tmpRet : -tmpRet;
  } else if( column == 1 ) {
    uint32_t tmp1 = reinterpret_cast< InternalData* >( item1.GetID() )->depth;
    uint32_t tmp2 = reinterpret_cast< InternalData* >( item2.GetID() )->depth;
    int tmpRet = int( tmp1 ) - int( tmp2 );
    return ascending ? tmpRet : -tmpRet;
  } else if( column == 2 ) {
    std::string tmp1 = reinterpret_cast< InternalData* >( item1.GetID() )->folderPath.string();
    std::string tmp2 = reinterpret_cast< InternalData* >( item2.GetID() )->folderPath.string();
    toLower( tmp1 );
    toLower( tmp2 );
    int tmpRet = tmp1.compare( tmp2 );
    return ascending ? tmpRet : -tmpRet;
  }
  return 0;
}

bool RepositoryModel::HasDefaultCompare() const {
  return true;
}

bool RepositoryModel::IsListModel() const {
  return false;
}

bool RepositoryModel::IsVirtualListModel() const {
  return false;
}

void RepositoryModel::ScanPath( std::filesystem::path const& path, uint32_t depth ) {
  git_repository* repo = nullptr;
  int error = git_repository_open( &repo, path.string().c_str() );
  if( error == 0 ) {
    // no error
    items_.push_back( Data( new InternalData{ .depth = depth, .folderPath = path, .gitRepo = std::shared_ptr< git_repository >( repo, []( git_repository* p ) { git_repository_free( p ); } ) } ) );
  } else if( error < 0 ) {
    // error
    git_error const* e = git_error_last();
    Log::Error( "Git error at '%s': %d/%d: %s", path.string().c_str(), error, e->klass, e->message );
  }
}
