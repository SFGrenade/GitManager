#include "models/RepositoryModel.hpp"

// Project headers
#include <Log.hpp>
#include <filesystem>
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

  for( auto const& item : items_ ) {
    ItemDeleted( wxDataViewItem(), reinterpret_cast< wxDataViewItem const& >( item ) );
  }
  items_.clear();

  int64_t sortId = 1;
  AddPath( basePath_, 0, sortId );
  ScanPath( basePath_, 0, sortId );
}

void RepositoryModel::GetValue( wxVariant& out_variant, wxDataViewItem const& wxdviItem, uint32_t col ) const {
  RepositoryModel::Data const& item = reinterpret_cast< RepositoryModel::Data const& >( wxdviItem );
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

bool RepositoryModel::HasValue( wxDataViewItem const& wxdviItem, uint32_t col ) const {
  // RepositoryModel::Data const& item = reinterpret_cast< RepositoryModel::Data const& >( wxdviItem );
  if( col < Columns::LAST ) {
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

  if( column == Columns::ALL ) {
    return item1->Compare( *item2.GetID() );
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

void RepositoryModel::ScanPath( std::filesystem::path const& path, uint32_t depth, int64_t& sortId ) {
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
    if( iter->path().filename().string().starts_with( "." ) ) {
      // ignore hidden folders for now
      continue;
    }
    entries.push_back( *iter );
  }

  entries.sort( []( std::filesystem::directory_entry const& a, std::filesystem::directory_entry const& b ) {
    std::string tmpA = a.path().filename().string();
    std::string tmpB = b.path().filename().string();
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
  if( RepositoryData::IsAllowed( path ) ) {
    Data tmp( new RepositoryData( sortId++, path ) );
    items_.push_back( tmp );
    ItemAdded( wxDataViewItem(), reinterpret_cast< wxDataViewItem const& >( tmp ) );
  }
}

void RepositoryModel::ModelItemsCyclicUpdate() {
  for( auto const& item : items_ ) {
    ValueChanged( reinterpret_cast< wxDataViewItem const& >( item ), Columns::CurrentBranch );
    ValueChanged( reinterpret_cast< wxDataViewItem const& >( item ), Columns::CurrentStatus );
  }
}
