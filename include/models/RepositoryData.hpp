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

  std::string GetFolderName() const;
  size_t GetDepth() const;
  std::string GetPath() const;
  std::string GetCurrentBranch() const;
  std::string GetCurrentStatus() const;

  int Compare( RepositoryData const& o ) const;

  protected:
  void PrintGitError( std::string const& type, int errorCode ) const;

  private:
  int64_t sortId_ = 0;
  std::filesystem::path folderPath_{};
  std::shared_ptr< git_repository > gitRepo_ = nullptr;
};
