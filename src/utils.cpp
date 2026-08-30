#include "utils.hpp"

namespace std {

std::string to_string( std::filesystem::path const& path ) {
  std::u8string tmp = path.u8string();
  return std::string( reinterpret_cast< char const* >( tmp.data() ), tmp.size() );
}

}  // namespace std
