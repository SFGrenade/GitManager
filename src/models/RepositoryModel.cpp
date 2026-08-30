#include "models/RepositoryModel.hpp"

// Project headers
#include <Log.hpp>
#include <utils.hpp>

// C++ headers
#include <list>

RepositoryModel::RepositoryModel() {
  Log::Trace( "RepositoryModel::RepositoryModel()" );
}

void RepositoryModel::SetBasePath( std::filesystem::path const& basePath ) {
  std::string basePathStr = std::to_string( basePath );
  Log::Trace( "RepositoryModel::SetBasePath( basePath: '%s' )", basePathStr.c_str() );

  basePath_ = basePath;

  for( auto const& item : items_ ) {
    ItemDeleted( wxDataViewItem(), reinterpret_cast< wxDataViewItem const& >( item ) );
  }
  items_.clear();

  int64_t sortId = 1;
  AddPath( basePath_, 0, sortId );
  ScanPath( basePath_, 0, sortId );
}

void RepositoryModel::GetValue( wxVariant& out_variant, wxDataViewItem const& wxdvItem, uint32_t col ) const {
  RepositoryModel::Data const& item = reinterpret_cast< RepositoryModel::Data const& >( wxdvItem );
  Log::Trace( "RepositoryModel::GetValue( out_variant, item: %p, col: %d )", item.GetID(), col );

  if( col == Columns::FolderName ) {
    out_variant = wxVariant( item->GetFolderName(), "FolderName" );
    // } else if( col == Columns::Depth ) {
    //   out_variant = wxVariant( std::to_string( item->GetDepth() ), "Depth" );
    // } else if( col == Columns::Path ) {
    //   out_variant = wxVariant( item->GetPath(), "Path" );
  } else if( col == Columns::CurrentBranch ) {
    out_variant = wxVariant( item->GetCurrentBranch(), "CurrentBranch" );
  } else if( col == Columns::CurrentStatus ) {
    out_variant = wxVariant( item->GetCurrentStatus(), "CurrentStatus" );
  }
}

bool RepositoryModel::HasValue( wxDataViewItem const& wxdvItem, uint32_t col ) const {
  RepositoryModel::Data const& item = reinterpret_cast< RepositoryModel::Data const& >( wxdvItem );
  Log::Trace( "RepositoryModel::HasValue( out_variant, item: %p, col: %d )", item.GetID(), col );

  if( col < Columns::LAST ) {
    return true;
  }
  if( !IsContainer( wxdvItem ) ) {
    return true;
  }
  if( HasContainerColumns( wxdvItem ) ) {
    return true;
  }
  return false;
}

bool RepositoryModel::SetValue( wxVariant const& variant, wxDataViewItem const& wxdvItem, uint32_t col ) {
  RepositoryModel::Data const& item = reinterpret_cast< RepositoryModel::Data const& >( wxdvItem );
  Log::Trace( "RepositoryModel::SetValue( variant: (%s), item: %p, col: %d )", variant.GetType().c_str().AsChar(), item.GetID(), col );

  return false;
}

bool RepositoryModel::GetAttr( wxDataViewItem const& wxdvItem, uint32_t col, wxDataViewItemAttr& out_attr ) const {
  RepositoryModel::Data const& item = reinterpret_cast< RepositoryModel::Data const& >( wxdvItem );
  Log::Trace( "RepositoryModel::GetAttr( item: %p, col: %d, out_attr )", item.GetID(), col );

  return false;
}

bool RepositoryModel::IsEnabled( wxDataViewItem const& wxdvItem, uint32_t col ) const {
  RepositoryModel::Data const& item = reinterpret_cast< RepositoryModel::Data const& >( wxdvItem );
  Log::Trace( "RepositoryModel::IsEnabled( out_variant, item: %p, col: %d )", item.GetID(), col );

  return true;
}

wxDataViewItem RepositoryModel::GetParent( wxDataViewItem const& wxdvItem ) const {
  RepositoryModel::Data const& item = reinterpret_cast< RepositoryModel::Data const& >( wxdvItem );
  Log::Trace( "RepositoryModel::GetParent( item: %p )", item.GetID() );

  return wxDataViewItem();
}

bool RepositoryModel::IsContainer( wxDataViewItem const& wxdvItem ) const {
  RepositoryModel::Data const& item = reinterpret_cast< RepositoryModel::Data const& >( wxdvItem );
  Log::Trace( "RepositoryModel::IsContainer( item: %p )", item.GetID() );

  return item.GetID() == nullptr;
}

bool RepositoryModel::HasContainerColumns( wxDataViewItem const& wxdvItem ) const {
  RepositoryModel::Data const& item = reinterpret_cast< RepositoryModel::Data const& >( wxdvItem );
  Log::Trace( "RepositoryModel::HasContainerColumns( item: %p )", item.GetID() );

  return false;
}

uint32_t RepositoryModel::GetChildren( wxDataViewItem const& wxdvItem, wxDataViewItemArray& out_children ) const {
  RepositoryModel::Data const& item = reinterpret_cast< RepositoryModel::Data const& >( wxdvItem );
  Log::Trace( "RepositoryModel::GetChildren( item: %p, out_children )", item.GetID() );

  if( item.GetID() == nullptr ) {
    for( auto const& child : items_ ) {
      out_children.Add( reinterpret_cast< wxDataViewItem const& >( child ) );
    }
    return items_.size();
  } else {
    return 0;
  }
}

void RepositoryModel::Resort() {
  Log::Trace( "RepositoryModel::Resort()" );
}

int32_t RepositoryModel::Compare( wxDataViewItem const& wxdvItem1, wxDataViewItem const& wxdvItem2, uint32_t column, bool ascending ) const {
  RepositoryModel::Data const& item1 = reinterpret_cast< RepositoryModel::Data const& >( wxdvItem1 );
  RepositoryModel::Data const& item2 = reinterpret_cast< RepositoryModel::Data const& >( wxdvItem2 );
  Log::Trace( "RepositoryModel::GetChildren( item1: %p, item2: %p, column: %d, ascending: %d )", item1.GetID(), item2.GetID(), column, ascending );

  if( !item1.IsOk() && item2.IsOk() ) {
    return -1;
  } else if( item1.IsOk() && !item2.IsOk() ) {
    return 1;
  }

  if( column == Columns::ALL ) {
    return item1->Compare( *item2.GetID() );
  }
  return 0;
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

void RepositoryModel::ScanPath( std::filesystem::path const& path, uint32_t depth, int64_t& sortId ) {
  std::string pathStr = std::to_string( path );
  Log::Trace( "RepositoryModel::ScanPath( path: '%s', depth: %d, sortId: %d )", pathStr.c_str(), depth, sortId );

  std::list< std::filesystem::directory_entry > entries;
  for( auto iter = std::filesystem::directory_iterator( path ); iter != std::filesystem::directory_iterator(); iter++ ) {
    if( !iter->is_directory() ) {
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
    std::string filenameStr = std::to_string( iter->path().filename() );
    if( filenameStr.starts_with( "." ) ) {
      // ignore hidden folders for now
      continue;
    }
    entries.push_back( *iter );
  }

  entries.sort( []( std::filesystem::directory_entry const& a, std::filesystem::directory_entry const& b ) {
    std::u8string tmpA = a.path().filename().u8string();
    std::u8string tmpB = b.path().filename().u8string();
    toLower( tmpA );
    toLower( tmpB );
    return tmpA < tmpB;
  } );

  for( auto const& entry : entries ) {
    AddPath( entry.path(), uint32_t( depth + 1 ), sortId );
  }

  for( auto const& entry : entries ) {
    ScanPath( entry.path(), uint32_t( depth + 1 ), sortId );
  }
}

void RepositoryModel::AddPath( std::filesystem::path const& path, uint32_t depth, int64_t& sortId ) {
  std::string pathStr = std::to_string( path );
  Log::Trace( "RepositoryModel::AddPath( path: '%s', depth: %d, sortId: %d )", pathStr.c_str(), depth, sortId );

  if( RepositoryData::IsAllowed( path ) ) {
    Data tmp( new RepositoryData( sortId++, path ) );
    items_.push_back( tmp );
    ItemAdded( wxDataViewItem(), reinterpret_cast< wxDataViewItem const& >( tmp ) );
  }
}

void RepositoryModel::ModelItemsCyclicUpdate() {
  Log::Trace( "RepositoryModel::ModelItemsCyclicUpdate()" );

  for( auto const& item : items_ ) {
    ValueChanged( reinterpret_cast< wxDataViewItem const& >( item ), Columns::CurrentBranch );
    ValueChanged( reinterpret_cast< wxDataViewItem const& >( item ), Columns::CurrentStatus );
  }
}
