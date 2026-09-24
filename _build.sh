#!/usr/bin/env bash

ORIGINAL_DIR=$(pwd)
SCRIPT_DIR=$(dirname "${BASH_SOURCE[0]}")

cd "$SCRIPT_DIR"

logFolder="./_build_logs"

doCommand () {
    logFile="$logFolder/$1.log"
    command="$2"
    touch "$logFile"
    echo "$(pwd)$ $command" >"$logFile" 2>&1
    eval "$command >>\"$logFile\" 2>&1"

    retCode=$?
    if [[ ! $retCode -eq 0 ]]; then
        cd "$ORIGINAL_DIR"
        exit $retCode
    fi
}

rm -r "$logFolder"
rm -r "./_dest"
rm -r "./.xmake"
rm -r "./build"

mkdir "$logFolder"

doCommand "00_made_build_logs" "echo we did it"

doCommand "01_xmake_set_theme" "xmake global --theme=plain"

#doCommand "02_xmake_configure_debug" "xmake config --toolchain=win-cross --target_os=windows --plat=cross --arch=x86_64 --mode=debug --kind=shared -vD --yes"

#doCommand "03_xmake_build_debug" "xmake build -a -vD"

doCommand "04_xmake_configure_release" "xmake config --toolchain=win-cross --target_os=windows --plat=cross --arch=x86_64 --mode=release --kind=shared -vD --yes"

doCommand "05_xmake_build_release" "xmake build -a -vD"

#doCommand "06_xmake_run" "xmake run -vD SfgGenerator '%ORIGINAL_DIR%'"

doCommand "07_xmake_install" "xmake install -vDo ./_dest -a"

#doCommand "10_xmake_test" "xmake test -vD SfgGenerator/*"

cd "$ORIGINAL_DIR"
