#include "models/DiffModel.hpp"

// Project headers
#include <Log.hpp>
#include <utils.hpp>

DiffModel::DiffModel() {
  Log::Trace( "DiffModel::DiffModel()" );
}

void DiffModel::SetRepository( RepositoryData* repository ) {
  Log::Trace( "DiffModel::SetRepository( repository: %p )", repository );

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
  Log::Trace( "DiffModel::GetDiff()" );

  return diff_;
}

void DiffModel::GetValue( wxVariant& out_variant, wxDataViewItem const& wxdvItem, uint32_t col ) const {
  DiffModel::Data const& item = reinterpret_cast< DiffModel::Data const& >( wxdvItem );
  Log::Trace( "DiffModel::GetValue( out_variant, item: %p, col: %d )", item.GetID(), col );

  if( col == Columns::Path ) {
    out_variant = wxVariant( item->GetPath(), "Path" );
  } else if( col == Columns::Status ) {
    out_variant = wxVariant( item->GetStatus(), "Status" );
  }
}

bool DiffModel::HasValue( wxDataViewItem const& wxdvItem, uint32_t col ) const {
  DiffModel::Data const& item = reinterpret_cast< DiffModel::Data const& >( wxdvItem );
  Log::Trace( "DiffModel::HasValue( item: %p, col: %d )", item.GetID(), col );

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

bool DiffModel::SetValue( wxVariant const& variant, wxDataViewItem const& wxdvItem, uint32_t col ) {
  DiffModel::Data const& item = reinterpret_cast< DiffModel::Data const& >( wxdvItem );
  Log::Trace( "DiffModel::SetValue( variant: (%s), item: %p, col: %d )", variant.GetType().c_str().AsChar(), item.GetID(), col );

  return false;
}

bool DiffModel::GetAttr( wxDataViewItem const& wxdvItem, uint32_t col, wxDataViewItemAttr& out_attr ) const {
  DiffModel::Data const& item = reinterpret_cast< DiffModel::Data const& >( wxdvItem );
  Log::Trace( "DiffModel::GetAttr( item: %p, col: %d, out_attr )", item.GetID(), col );

  return false;
}

bool DiffModel::IsEnabled( wxDataViewItem const& wxdvItem, uint32_t col ) const {
  DiffModel::Data const& item = reinterpret_cast< DiffModel::Data const& >( wxdvItem );
  Log::Trace( "DiffModel::IsEnabled( item: %p, col: %d )", item.GetID(), col );

  return true;
}

wxDataViewItem DiffModel::GetParent( wxDataViewItem const& wxdvItem ) const {
  DiffModel::Data const& item = reinterpret_cast< DiffModel::Data const& >( wxdvItem );
  Log::Trace( "DiffModel::GetParent( item: %p )", item.GetID() );

  return wxDataViewItem();
}

bool DiffModel::IsContainer( wxDataViewItem const& wxdvItem ) const {
  DiffModel::Data const& item = reinterpret_cast< DiffModel::Data const& >( wxdvItem );
  Log::Trace( "DiffModel::IsContainer( item: %p )", item.GetID() );

  return item.GetID() == nullptr;
}

bool DiffModel::HasContainerColumns( wxDataViewItem const& wxdvItem ) const {
  DiffModel::Data const& item = reinterpret_cast< DiffModel::Data const& >( wxdvItem );
  Log::Trace( "DiffModel::HasContainerColumns( item: %p )", item.GetID() );

  return false;
}

uint32_t DiffModel::GetChildren( wxDataViewItem const& wxdvItem, wxDataViewItemArray& out_children ) const {
  DiffModel::Data const& item = reinterpret_cast< DiffModel::Data const& >( wxdvItem );
  Log::Trace( "DiffModel::GetChildren( item: %p, out_children )", item.GetID() );

  if( item.GetID() == nullptr ) {
    for( auto const& child : items_ ) {
      out_children.Add( reinterpret_cast< wxDataViewItem const& >( child ) );
    }
    return items_.size();
  } else {
    return 0;
  }
}

void DiffModel::Resort() {
  Log::Trace( "DiffModel::Resort()" );
}

int32_t DiffModel::Compare( wxDataViewItem const& wxdvItem1, wxDataViewItem const& wxdvItem2, uint32_t column, bool ascending ) const {
  DiffModel::Data const& item1 = reinterpret_cast< DiffModel::Data const& >( wxdvItem1 );
  DiffModel::Data const& item2 = reinterpret_cast< DiffModel::Data const& >( wxdvItem2 );
  Log::Trace( "DiffModel::Compare( item1: %p, item2: %p, column: %d, ascending: %d )", item1.GetID(), item2.GetID(), column, ascending );


  if( !item1.IsOk() && item2.IsOk() ) {
    return -1;
  } else if( item1.IsOk() && !item2.IsOk() ) {
    return 1;
  }

  if( column == static_cast< decltype( column ) >( Columns::ALL ) ) {
    return item1->Compare( *item2.GetID() );
  }
  return 0;
}

bool DiffModel::HasDefaultCompare() const {
  Log::Trace( "DiffModel::HasDefaultCompare()" );

  return true;
}

bool DiffModel::IsListModel() const {
  Log::Trace( "DiffModel::IsListModel()" );

  return false;
}

bool DiffModel::IsVirtualListModel() const {
  Log::Trace( "DiffModel::IsVirtualListModel()" );

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
