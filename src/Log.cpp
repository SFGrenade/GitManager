#include "Log.hpp"

FILE* Log::file_ = nullptr;

void Log::Init() {
  file_ = fopen( "out.log", "w" );
}
