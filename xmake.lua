set_project( "GitManager" )

set_version( "0.0.1", { build = "%Y%m%d", soname = true } )

add_rules( "mode.debug", "mode.release", "mode.releasedbg", "mode.minsizerel" )
add_rules( "plugin.compile_commands.autoupdate", { outputdir = ".vscode" } )

set_languages( "c++20" )

add_repositories( "custom xmake-repo", { rootdir = os.scriptdir() } )

if is_plat( "windows" ) then
    add_cxflags( "/Zc:__cplusplus" )
    add_cxflags( "/Zc:preprocessor" )

    add_cxflags( "/permissive-" )

    add_ldflags( "-force", { force = true } )

    --set_runtimes( is_mode( "debug" ) and "MDd" or "MD" )
else
end

set_warnings( "allextra" )

add_requireconfs( "**", "*.**", { system = false } )
add_requireconfs( "*", { configs = { shared = get_config( "kind" ) == "shared" } } )

add_requires( "libgit2" )
add_requires( "wxwidgets" )

add_requireconfs( "libgit2", { configs = { shared = false, ssh = true } } )

toolchain( "win-cross" )
    set_kind( "standalone" )
    set_toolset( "cc",  "clang-cl" )
    set_toolset( "cxx", "clang-cl" )
    set_toolset( "as",  "clang-cl" )
    set_toolset( "cl",  "clang-cl" )
    set_toolset( "ar",  "lld-link" )
    set_toolset( "ld",  "lld-link" )
    set_toolset( "sh",  "lld-link" )
    set_toolset( "as",  "llvm-lib" )

    -- check toolchain
    on_check( function ( toolchain )
        return import( "lib.detect.find_tool" )( "clang" )
    end )
toolchain_end()

target( "GitManager" )
    set_kind( "binary" )

    set_default( true )
    set_group( "EXES" )

    if is_plat( "linux" ) or is_os( "linux" ) then
        add_defines( "GM_LINUX" )
    elseif is_plat( "macos" ) or is_os( "macos" ) then
        add_defines( "GM_MACOS" )
    elseif is_plat( "windows" ) or is_os( "windows" ) then
        add_defines( "GM_WINDOWS" )
    end

    add_packages( "libgit2", { public = true } )
    add_packages( "wxwidgets", { public = true } )

    add_includedirs( "include", "$(builddir)", { public = true } )

    add_headerfiles( "include/(models/*.hpp)" )
    add_headerfiles( "include/(ui/*.hpp)" )
    add_headerfiles( "include/(*.hpp)" )

    add_files( "resources/*" )

    add_files( "src/models/*.cpp" )
    add_files( "src/ui/*.cpp" )
    add_files( "src/*.cpp" )
target_end()
