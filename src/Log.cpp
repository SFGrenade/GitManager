#include "Log.hpp"

// C++ headers
#include <ctime>

FILE* Log::file_ = nullptr;
Log::Level Log::level_ = Log::Level::All;

void Log::Init() {
  // todo: fixme: have some logging directory depending on the system
  file_ = fopen( "out.log", "w" );
}

void Log::SetLevel( Level level ) {
  level_ = level;
}

void Log::DeInit() {
  fclose( file_ );
  file_ = nullptr;
}

void Log::LogLine( std::string const& level, std::string const& outputString ) {
  StartLogLine( level );
  std::fprintf( file_, outputString.c_str() );
  EndLogLine();
}

void Log::Trace( std::string const& outputString ) {
  if( level_ <= Log::Level::Trace )
    return LogLine( "trace", outputString );
}

void Log::Debug( std::string const& outputString ) {
  if( level_ <= Log::Level::Debug )
    return LogLine( "debug", outputString );
}

void Log::Warning( std::string const& outputString ) {
  if( level_ <= Log::Level::Warning )
    return LogLine( "warning", outputString );
}

void Log::Error( std::string const& outputString ) {
  if( level_ <= Log::Level::Error )
    return LogLine( "error", outputString );
}

void Log::StartLogLine( std::string const& level ) {
  std::time_t currentTime = std::time( nullptr );
  std::tm* localCurrentTime = std::localtime( &currentTime );
  std::fprintf( file_, "[%04d-%02d-%02d %02d:%02d:%02d] [%s] ", localCurrentTime->tm_year + 1900, localCurrentTime->tm_mon + 1, localCurrentTime->tm_mday, localCurrentTime->tm_hour, localCurrentTime->tm_min, localCurrentTime->tm_sec, level.c_str() );
}

void Log::EndLogLine() {
  std::fprintf( file_, "\n" );
  std::fflush( file_ );
}
