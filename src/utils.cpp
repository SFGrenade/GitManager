#include "utils.hpp"

namespace std {

std::string to_string( std::filesystem::path const& path ) {
  std::u8string tmp = path.u8string();
  return std::string( tmp.begin(), tmp.end() );
}

}  // namespace std
