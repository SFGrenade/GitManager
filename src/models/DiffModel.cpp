#include "models/DiffModel.hpp"

// Project headers
#include <Log.hpp>
#include <utils.hpp>

DiffModel::DiffModel() {
  Log::Trace( "DiffModel::DiffModel()" );
}

void DiffModel::SetRepository( RepositoryData* repository ) {
  repository_ = repository;

  for( auto const& item : items_ ) {
    ItemDeleted( wxDataViewItem(), reinterpret_cast< wxDataViewItem const& >( item ) );
  }
  items_.clear();

  if( repository_ == nullptr ) {
    return;
  }

  sortId_ = 0;

  git_diff_options diffOpts;
  if( int error = git_diff_options_init( &diffOpts, GIT_DIFF_OPTIONS_VERSION ); error < 0 ) {
    PrintGitError( "git_diff_options_init", error );
    return;
  }
  diffOpts.flags = GIT_DIFF_NORMAL | GIT_DIFF_INCLUDE_UNTRACKED | GIT_DIFF_RECURSE_UNTRACKED_DIRS | GIT_DIFF_INCLUDE_TYPECHANGE | GIT_DIFF_INCLUDE_TYPECHANGE_TREES | GIT_DIFF_SKIP_BINARY_CHECK | GIT_DIFF_INCLUDE_UNREADABLE;

  git_diff* r_diff = nullptr;
  if( int error = git_diff_index_to_workdir( &r_diff, repository_->GetRepo().get(), nullptr, &diffOpts ); error < 0 ) {
    PrintGitError( "git_diff_index_to_workdir", error );
    return;
  }
  diff_ = std::shared_ptr< git_diff >( r_diff, []( git_diff* p ) {
    if( p )
      git_diff_free( p );
  } );

  if( int error = git_diff_foreach( diff_.get(), GitDiffFileForeach, nullptr, nullptr, nullptr, this ); error < 0 ) {
    PrintGitError( "git_diff_foreach", error );
    return;
  }
}

std::shared_ptr< git_diff > DiffModel::GetDiff() const {
  return diff_;
}

void DiffModel::GetValue( wxVariant& out_variant, wxDataViewItem const& wxdviItem, uint32_t col ) const {
  DiffModel::Data const& item = reinterpret_cast< DiffModel::Data const& >( wxdviItem );
  if( col == Columns::Path ) {
    out_variant = wxVariant( item->GetPath(), "Path" );
  } else if( col == Columns::Status ) {
    out_variant = wxVariant( item->GetStatus(), "Status" );
  }
}

bool DiffModel::HasValue( wxDataViewItem const& wxdviItem, uint32_t col ) const {
  // DiffModel::Data const& item = reinterpret_cast< DiffModel::Data const& >( wxdviItem );
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

bool DiffModel::SetValue( wxVariant const& /*variant*/, wxDataViewItem const& /*wxdviItem*/, uint32_t /*col*/ ) {
  // DiffModel::Data const& item = reinterpret_cast< DiffModel::Data const& >( wxdviItem );
  return false;
}

bool DiffModel::GetAttr( wxDataViewItem const& /*wxdviItem*/, uint32_t /*col*/, wxDataViewItemAttr& /*out_attr*/ ) const {
  // DiffModel::Data const& item = reinterpret_cast< DiffModel::Data const& >( wxdviItem );
  return false;
}

bool DiffModel::IsEnabled( wxDataViewItem const& /*wxdviItem*/, uint32_t /*col*/ ) const {
  // DiffModel::Data const& item = reinterpret_cast< DiffModel::Data const& >( wxdviItem );
  return true;
}

wxDataViewItem DiffModel::GetParent( wxDataViewItem const& /*wxdviItem*/ ) const {
  // DiffModel::Data const& item = reinterpret_cast< DiffModel::Data const& >( wxdviItem );
  return wxDataViewItem();
}

bool DiffModel::IsContainer( wxDataViewItem const& wxdviItem ) const {
  DiffModel::Data const& item = reinterpret_cast< DiffModel::Data const& >( wxdviItem );
  return item.GetID() == nullptr;
}

bool DiffModel::HasContainerColumns( wxDataViewItem const& /*wxdviItem*/ ) const {
  // DiffModel::Data const& item = reinterpret_cast< DiffModel::Data const& >( wxdviItem );
  return false;
}

uint32_t DiffModel::GetChildren( wxDataViewItem const& wxdviItem, wxDataViewItemArray& out_children ) const {
  DiffModel::Data const& item = reinterpret_cast< DiffModel::Data const& >( wxdviItem );

  if( item.GetID() == nullptr ) {
    for( auto const& item : items_ ) {
      out_children.Add( reinterpret_cast< wxDataViewItem const& >( item ) );
    }
    return items_.size();
  } else {
    return 0;
  }
}

void DiffModel::Resort() {}

int32_t DiffModel::Compare( wxDataViewItem const& wxdviItem1, wxDataViewItem const& wxdviItem2, uint32_t column, bool /*ascending*/ ) const {
  DiffModel::Data const& item1 = reinterpret_cast< DiffModel::Data const& >( wxdviItem1 );
  DiffModel::Data const& item2 = reinterpret_cast< DiffModel::Data const& >( wxdviItem2 );

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

bool DiffModel::HasDefaultCompare() const {
  return true;
}

bool DiffModel::IsListModel() const {
  return false;
}

bool DiffModel::IsVirtualListModel() const {
  return false;
}

int DiffModel::GitDiffFileForeach( git_diff_delta const* delta, float progress, void* user ) {
  Log::Trace( "DiffModel::GitDiffFileForeach( delta: %p, progress: %f, user: %p )", delta, progress, user );
  DiffModel* self = reinterpret_cast< DiffModel* >( user );

  Data tmp( new DiffData( self->sortId_++, delta ) );
  self->items_.push_back( tmp );
  self->ItemAdded( wxDataViewItem(), reinterpret_cast< wxDataViewItem const& >( tmp ) );

  return 0;
}

void DiffModel::PrintGitError( std::string const& type, int errorCode ) const {
  git_error const* e = git_error_last();
  Log::Error( "%s error with '%s': %d/%d: %s", type.c_str(), repository_->GetFolderName().c_str(), errorCode, e->klass, e->message );
}
