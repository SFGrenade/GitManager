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

  git_diff* diff = nullptr;
  if( int error = git_diff_index_to_workdir( &diff, gitRepo_.get(), nullptr, nullptr ); error < 0 ) {
    PrintGitError( "git_diff_index_to_workdir", error );
  }

  std::string tmpStr;
  if( diff == nullptr ) {
    tmpStr = "-";
  } else {
    tmpStr = std::to_string( git_diff_num_deltas( diff ) );
  }

  if( diff ) {
    git_diff_free( diff );
  }

  std::array< char, 512 > tmpBuffer;
  tmpBuffer.fill( '\0' );

  return tmpStr;
}

int RepositoryData::Compare( RepositoryData const& o ) const {
  Log::Trace( "RepositoryData::Compare( o: {'%s'} )", o.GetPath().c_str() );

  return sortId_ - o.sortId_;
}

void RepositoryData::PrintGitError( std::string const& type, int errorCode ) const {
  git_error const* e = git_error_last();
  Log::Error( "%s error with '%s': %d/%d: %s", type.c_str(), GetFolderName().c_str(), errorCode, e->klass, e->message );
}
