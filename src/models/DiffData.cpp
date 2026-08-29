#include "models/DiffData.hpp"

// Project headers
#include <Log.hpp>
#include <utils.hpp>

DiffData::DiffData( int64_t sortId, git_diff_delta const* delta ) : sortId_( sortId ), delta_( delta ) {
  Log::Trace( "DiffData::DiffData( sortId: %d, delta: %p )", sortId, delta );
}

std::string DiffData::GetPath() const {
  Log::Trace( "DiffData::GetPath()" );

  return delta_->new_file.path;
}

std::string DiffData::GetStatus() const {
  Log::Trace( "DiffData::GetStatus()" );

  switch( delta_->status ) {
    case GIT_DELTA_UNMODIFIED:
      return "Unmodified";
    case GIT_DELTA_ADDED:
      return "Added";
    case GIT_DELTA_DELETED:
      return "Deleted";
    case GIT_DELTA_MODIFIED:
      return "Modified";
    case GIT_DELTA_RENAMED:
      return "Renamed";
    case GIT_DELTA_COPIED:
      return "Copied";
    case GIT_DELTA_IGNORED:
      return "Ignored";
    case GIT_DELTA_UNTRACKED:
      return "Untracked";
    case GIT_DELTA_TYPECHANGE:
      return "Typechange";
    case GIT_DELTA_UNREADABLE:
      return "Unreadable";
    case GIT_DELTA_CONFLICTED:
      return "Conflicted";
    default:
      return "Unknown";
  }
}

int DiffData::Compare( DiffData const& o ) const {
  Log::Trace( "DiffData::Compare( o: {'%s'} )", o.GetPath().c_str() );

  return sortId_ - o.sortId_;
}
