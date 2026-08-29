#pragma once

// Library headers
#include <git2.h>

// C++ headers
#include <cstdint>
#include <string>

class DiffData {
  public:
  explicit DiffData( int64_t sortId, git_diff_delta const* delta );

  public:
  std::string GetPath() const;
  std::string GetStatus() const;

  int Compare( DiffData const& o ) const;

  private:
  int64_t sortId_ = 0;
  git_diff_delta const* delta_ = nullptr;
};
