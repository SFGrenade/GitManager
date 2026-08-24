#include "models/RepositoryModel.hpp"

// Project headers
#include <Log.hpp>
#include <utils.hpp>

// Library headers
#include <wx/log.h>

RepositoryModel::RepositoryModel() {
  Log::Trace( "RepositoryModel::RepositoryModel()" );
}

void RepositoryModel::SetBasePath( std::filesystem::path const& basePath ) {
  basePath_ = basePath;

  for( auto const& item : items_ ) {
    ItemDeleted( wxDataViewItem(), reinterpret_cast< wxDataViewItem const& >( item ) );
  }
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
}

void RepositoryModel::GetValue( wxVariant& out_variant, wxDataViewItem const& wxdviItem, uint32_t col ) const {
  RepositoryModel::Data const& item = reinterpret_cast< RepositoryModel::Data const& >( wxdviItem );
  if( col == 0 ) {
    out_variant = wxVariant( item->folderPath.filename().string(), "Folder Name" );
  } else if( col == 1 ) {
    out_variant = wxVariant( std::to_string( item->depth ), "Depth" );
  } else if( col == 2 ) {
    out_variant = wxVariant( item->folderPath.string(), "Path" );
  }
}

bool RepositoryModel::HasValue( wxDataViewItem const& wxdviItem, uint32_t col ) const {
  // RepositoryModel::Data const& item = reinterpret_cast< RepositoryModel::Data const& >( wxdviItem );
  if( col < 3 ) {
    return true;
  }
  if( !IsContainer( wxdviItem ) ) {
    return true;
  }
  if( HasContainerColumns( wxdviItem ) ) {
    return true;
  }
  return false;
}

bool RepositoryModel::SetValue( wxVariant const& /*variant*/, wxDataViewItem const& /*wxdviItem*/, uint32_t /*col*/ ) {
  // RepositoryModel::Data const& item = reinterpret_cast< RepositoryModel::Data const& >( wxdviItem );
  return false;
}

bool RepositoryModel::GetAttr( wxDataViewItem const& /*wxdviItem*/, uint32_t /*col*/, wxDataViewItemAttr& /*out_attr*/ ) const {
  // RepositoryModel::Data const& item = reinterpret_cast< RepositoryModel::Data const& >( wxdviItem );
  return false;
}

bool RepositoryModel::IsEnabled( wxDataViewItem const& /*wxdviItem*/, uint32_t /*col*/ ) const {
  // RepositoryModel::Data const& item = reinterpret_cast< RepositoryModel::Data const& >( wxdviItem );
  return true;
}

wxDataViewItem RepositoryModel::GetParent( wxDataViewItem const& /*wxdviItem*/ ) const {
  // RepositoryModel::Data const& item = reinterpret_cast< RepositoryModel::Data const& >( wxdviItem );
  return wxDataViewItem();
}

bool RepositoryModel::IsContainer( wxDataViewItem const& wxdviItem ) const {
  RepositoryModel::Data const& item = reinterpret_cast< RepositoryModel::Data const& >( wxdviItem );
  return item.GetID() == nullptr;
}

bool RepositoryModel::HasContainerColumns( wxDataViewItem const& /*wxdviItem*/ ) const {
  // RepositoryModel::Data const& item = reinterpret_cast< RepositoryModel::Data const& >( wxdviItem );
  return false;
}

uint32_t RepositoryModel::GetChildren( wxDataViewItem const& wxdviItem, wxDataViewItemArray& out_children ) const {
  RepositoryModel::Data const& item = reinterpret_cast< RepositoryModel::Data const& >( wxdviItem );

  if( item.GetID() == nullptr ) {
    for( auto const& item : items_ ) {
      out_children.Add( reinterpret_cast< wxDataViewItem const& >( item ) );
    }
    return items_.size();
  } else {
    return 0;
  }
}

void RepositoryModel::Resort() {}

int32_t RepositoryModel::Compare( wxDataViewItem const& wxdviItem1, wxDataViewItem const& wxdviItem2, uint32_t column, bool /*ascending*/ ) const {
  RepositoryModel::Data const& item1 = reinterpret_cast< RepositoryModel::Data const& >( wxdviItem1 );
  RepositoryModel::Data const& item2 = reinterpret_cast< RepositoryModel::Data const& >( wxdviItem2 );

  if( !item1.IsOk() && item2.IsOk() ) {
    return -1;
  } else if( item1.IsOk() && !item2.IsOk() ) {
    return 1;
  }

  if( column == -1 ) {
    // default
    if( item1->depth < item2->depth ) {
      return -1;
    } else if( item1->depth > item2->depth ) {
      return 1;
    }
    std::string path1 = item1->folderPath.string();
    std::string path2 = item2->folderPath.string();
    toLower( path1 );
    toLower( path2 );
    return path1.compare( path2 );
  } else if( column == 0 ) {
    std::string tmp1 = item1->folderPath.filename().string();
    std::string tmp2 = item2->folderPath.filename().string();
    toLower( tmp1 );
    toLower( tmp2 );
    return tmp1.compare( tmp2 );
  } else if( column == 1 ) {
    uint32_t tmp1 = item1->depth;
    uint32_t tmp2 = item2->depth;
    return int( tmp1 ) - int( tmp2 );
  } else if( column == 2 ) {
    std::string tmp1 = item1->folderPath.string();
    std::string tmp2 = item2->folderPath.string();
    toLower( tmp1 );
    toLower( tmp2 );
    return tmp1.compare( tmp2 );
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
    Data tmp( new InternalData{ .depth = depth, .folderPath = path, .gitRepo = std::shared_ptr< git_repository >( repo, []( git_repository* p ) { git_repository_free( p ); } ) } );
    items_.push_back( tmp );
    ItemAdded( wxDataViewItem(), reinterpret_cast< wxDataViewItem const& >( tmp ) );
  } else if( error < 0 ) {
    // error
    git_error const* e = git_error_last();
    Log::Error( "Git error at '%s': %d/%d: %s", path.string().c_str(), error, e->klass, e->message );
  }
}
