#include "Log.hpp"

// Project headers
#include <utils.hpp>

// C++ headers
#include <ctime>
#include <filesystem>

FILE* Log::file_ = nullptr;
Log::Level Log::level_ = Log::Level::All;

void Log::Init() {
  std::filesystem::path logFolder = "";
  std::string logFile = "out.log";
#if defined( GM_LINUX )
  if( char* xdgDataHome = std::getenv( "XDG_DATA_HOME" ); xdgDataHome != nullptr ) {
    logFolder = std::filesystem::path( xdgDataHome ) / "GitManager";
  } else if( char* home = std::getenv( "HOME" ); home != nullptr ) {
    logFolder = std::filesystem::path( home ) / ".local" / "share" / "GitManager";
  }
#elif defined( GM_MACOS )
  if( char* home = std::getenv( "HOME" ); home != nullptr ) {
    logFolder = std::filesystem::path( home ) / "Library" / "Logs" / "GitManager";
  }
#elif defined( GM_WINDOWS )
  if( char* home = std::getenv( "LOCALAPPDATA" ); home != nullptr ) {
    logFolder = std::filesystem::path( home ) / "GitManager";
  }
#endif
  std::filesystem::create_directories( logFolder );
  file_ = fopen( std::to_string( logFolder / logFile ).c_str(), "w" );
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
