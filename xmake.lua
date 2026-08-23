set_project( "FL-Studio-Plugins" )

set_version( "0.0.1", { build = "%Y%m%d", soname = true } )

add_rules( "mode.debug", "mode.release", "mode.releasedbg", "mode.minsizerel" )
add_rules( "plugin.compile_commands.autoupdate", { outputdir = ".vscode" } )

set_languages( "c++20" )

if is_plat( "windows" ) then
    add_cxflags( "/Zc:__cplusplus" )
    add_cxflags( "/Zc:preprocessor" )

    add_cxflags( "/permissive-" )

    add_ldflags( "-force", { force = true } )

    --set_runtimes( is_mode( "debug" ) and "MDd" or "MD" )
else
end

set_warnings( "everything" )

add_requireconfs( "**", "*.**", { system = false } )
add_requireconfs( "*", { configs = { shared = get_config( "kind" ) == "shared" } } )

add_requires( "libgit2" )
add_requires( "wxwidgets" )

add_requireconfs( "libgit2", { configs = { shared = false, ssh = true } } )

target( "GitManager" )
    set_kind( "binary" )

    set_default( true )
    set_group( "EXES" )

    add_packages( "libgit2", { public = true } )
    add_packages( "wxwidgets", { public = true } )

    add_includedirs( "include", "$(builddir)", { public = true } )

    add_headerfiles( "include/(*.hpp)" )
    add_headerfiles( "include/(ui/*.hpp)" )

    add_files( "src/*.cpp" )
    add_files( "src/ui/*.cpp" )
