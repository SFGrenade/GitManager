#include "models/RepositoryData.hpp"

// Project headers
#include <Log.hpp>
#include <filesystem>
#include <utils.hpp>

// C++ headers
#include <array>
#include <iterator>

bool RepositoryData::IsAllowed( std::filesystem::path const& path ) {
  // check for git repository
  {
    git_repository* repo = nullptr;
    if( int error = git_repository_open( &repo, path.string().c_str() ); error == 0 ) {
      git_repository_free( repo );
      return true;
    }
  }
  return false;
}

RepositoryData::RepositoryData( int64_t sortId, std::filesystem::path const& path ) : sortId_( sortId ), folderPath_( std::filesystem::absolute( path ) ) {
  Log::Trace( "RepositoryData::RepositoryData( path: '%s' )", path.string().c_str() );

  git_repository* repo = nullptr;
  if( int error = git_repository_open( &repo, folderPath_.string().c_str() ); error < 0 ) {
    PrintGitError( "git_repository_open", error );
    return;
  }
  gitRepo_ = std::shared_ptr< git_repository >( repo, []( git_repository* p ) {
    if( p )
      git_repository_free( p );
  } );
}

std::string RepositoryData::GetFolderName() const {
  Log::Trace( "RepositoryData::GetFolderName()" );

  return folderPath_.filename().string();
}

size_t RepositoryData::GetDepth() const {
  Log::Trace( "RepositoryData::GetDepth()" );

  return std::distance( folderPath_.begin(), folderPath_.end() );
}

std::string RepositoryData::GetPath() const {
  Log::Trace( "RepositoryData::GetPath()" );

  return folderPath_.string();
}

std::string RepositoryData::GetCurrentBranch() const {
  Log::Trace( "RepositoryData::GetCurrentBranch()" );

  git_reference* ref = nullptr;
  int error = git_repository_head( &ref, gitRepo_.get() );
  if( error < 0 ) {
    PrintGitError( "git_repository_head", error );
  }

  std::string tmpStr;
  if( ( error == GIT_ENOTFOUND ) || ( ref == nullptr ) ) {
    tmpStr = "-";
  } else if( git_reference_is_tag( ref ) || git_reference_is_branch( ref ) ) {
    // branch/tag checked out, probably
    tmpStr = git_reference_shorthand( ref );
  } else {
    tmpStr = git_reference_name( ref );
  }

  if( ref ) {
    git_reference_free( ref );
  }
  return tmpStr;
}

std::string RepositoryData::GetCurrentStatus() const {
  Log::Trace( "RepositoryData::GetCurrentStatus()" );

  git_diff_options diffOpts;
  if( int error = git_diff_options_init( &diffOpts, GIT_DIFF_OPTIONS_VERSION ); error < 0 ) {
    PrintGitError( "git_diff_options_init", error );
  }
  diffOpts.flags = GIT_DIFF_NORMAL | GIT_DIFF_INCLUDE_UNTRACKED | GIT_DIFF_RECURSE_UNTRACKED_DIRS | GIT_DIFF_INCLUDE_TYPECHANGE | GIT_DIFF_INCLUDE_TYPECHANGE_TREES | GIT_DIFF_SKIP_BINARY_CHECK | GIT_DIFF_INCLUDE_UNREADABLE;
  git_diff* diff = nullptr;
  if( int error = git_diff_index_to_workdir( &diff, gitRepo_.get(), nullptr, &diffOpts ); error < 0 ) {
    PrintGitError( "git_diff_index_to_workdir", error );
  }

  size_t numUnmodified = 0;  // no changes
  size_t numAdded = 0;       // entry does not exist in old version
  size_t numDeleted = 0;     // entry does not exist in new version
  size_t numModified = 0;    // entry content changed between old and new
  size_t numRenamed = 0;     // entry was renamed between old and new
  size_t numCopied = 0;      // entry was copied from another old entry
  size_t numIgnored = 0;     // entry is ignored item in workdir
  size_t numUntracked = 0;   // entry is untracked item in workdir
  size_t numTypechange = 0;  // type of entry changed between old and new
  size_t numUnreadable = 0;  // entry is unreadable
  size_t numConflicted = 0;  // entry in the index is conflicted

  if( diff != nullptr ) {
    numUnmodified = git_diff_num_deltas_of_type( diff, GIT_DELTA_UNMODIFIED );
    numAdded = git_diff_num_deltas_of_type( diff, GIT_DELTA_ADDED );
    numDeleted = git_diff_num_deltas_of_type( diff, GIT_DELTA_DELETED );
    numModified = git_diff_num_deltas_of_type( diff, GIT_DELTA_MODIFIED );
    numRenamed = git_diff_num_deltas_of_type( diff, GIT_DELTA_RENAMED );
    numCopied = git_diff_num_deltas_of_type( diff, GIT_DELTA_COPIED );
    numIgnored = git_diff_num_deltas_of_type( diff, GIT_DELTA_IGNORED );
    numUntracked = git_diff_num_deltas_of_type( diff, GIT_DELTA_UNTRACKED );
    numTypechange = git_diff_num_deltas_of_type( diff, GIT_DELTA_TYPECHANGE );
    numUnreadable = git_diff_num_deltas_of_type( diff, GIT_DELTA_UNREADABLE );
    numConflicted = git_diff_num_deltas_of_type( diff, GIT_DELTA_CONFLICTED );
  }

  if( diff ) {
    git_diff_free( diff );
  }

  std::array< char, 512 > tmpBuffer;
  tmpBuffer.fill( '\0' );

  size_t stringString = snprintf( tmpBuffer.data(),
                                  tmpBuffer.size() - 1,
                                  "%d %c, %d %c, %d %c, %d %c, %d %c, %d %c, %d %c, %d %c, %d %c",
                                  numAdded,
                                  git_diff_status_char( GIT_DELTA_ADDED ),
                                  numDeleted,
                                  git_diff_status_char( GIT_DELTA_DELETED ),
                                  numModified,
                                  git_diff_status_char( GIT_DELTA_MODIFIED ),
                                  numRenamed,
                                  git_diff_status_char( GIT_DELTA_RENAMED ),
                                  numCopied,
                                  git_diff_status_char( GIT_DELTA_COPIED ),
                                  numUntracked,
                                  git_diff_status_char( GIT_DELTA_UNTRACKED ),
                                  numTypechange,
                                  git_diff_status_char( GIT_DELTA_TYPECHANGE ),
                                  numUnreadable,
                                  git_diff_status_char( GIT_DELTA_UNREADABLE ),
                                  numConflicted,
                                  git_diff_status_char( GIT_DELTA_CONFLICTED ) );

  return std::string( tmpBuffer.data(), stringString );
}

int RepositoryData::Compare( RepositoryData const& o ) const {
  Log::Trace( "RepositoryData::Compare( o: {'%s'} )", o.GetPath().c_str() );

  return sortId_ - o.sortId_;
}

void RepositoryData::PrintGitError( std::string const& type, int errorCode ) const {
  git_error const* e = git_error_last();
  Log::Error( "%s error with '%s': %d/%d: %s", type.c_str(), GetFolderName().c_str(), errorCode, e->klass, e->message );
}
