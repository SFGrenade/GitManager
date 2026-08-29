#pragma once

// Library headers
#include <git2.h>

// C++ headers
#include <cstdint>
#include <filesystem>
#include <string>

class RepositoryData {
  public:
  static bool IsAllowed( std::filesystem::path const& path );

  public:
  explicit RepositoryData( int64_t sortId, std::filesystem::path const& path );

  public:
  /* git functionality */
  void TrackFile( std::filesystem::path const& path );      // todo: fixme: how should this look?
  void UntrackFile( std::filesystem::path const& path );    // todo: fixme: how should this look?
  void RevertFile( std::filesystem::path const& path );     // todo: fixme: how should this look?
  void StageFile( std::filesystem::path const& path );      // todo: fixme: how should this look?
  void UnstageFile( std::filesystem::path const& path );    // todo: fixme: how should this look?

  void Fetch();   // todo: fixme: how should this look?
  void Pull();    // todo: fixme: how should this look?
  void Commit();  // todo: fixme: how should this look?
  void Push();    // todo: fixme: how should this look?

  void CreateBranch( std::string const& branchName );    // todo: fixme: how should this look?
  void CheckoutBranch( std::string const& branchName );  // todo: fixme: how should this look?
  void DeleteBranch( std::string const& branchName );    // todo: fixme: how should this look?
  /* git functionality */

  std::string GetFolderName() const;
  size_t GetDepth() const;
  std::string GetPath() const;
  std::string GetCurrentBranch() const;
  std::string GetCurrentStatus() const;
  std::shared_ptr< git_repository > GetRepo() const;

  int Compare( RepositoryData const& o ) const;

  protected:
  void PrintGitError( std::string const& type, int errorCode ) const;

  private:
  int64_t sortId_ = 0;
  std::filesystem::path folderPath_{};
  std::shared_ptr< git_repository > gitRepo_ = nullptr;
};
