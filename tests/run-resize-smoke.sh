#!/usr/bin/env bash
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR"
if [[ "${1:-}" != --inside ]]; then
    exec docker run --rm --platform linux/amd64 \
        --user "$(id -u):$(id -g)" \
        --mount "type=bind,src=$ROOT_DIR,dst=$ROOT_DIR" \
        --workdir "$ROOT_DIR" --entrypoint /bin/bash \
        wischner/gcc-x86_64-linux-x11:latest "$0" --inside
fi
export OPENWINHOME="$ROOT_DIR/bin"
export LD_LIBRARY_PATH="$OPENWINHOME/lib"
test_dir="$(mktemp -d /tmp/openlook-resize.XXXXXX)"
cleanup() {
    test_status=$?
    if ((test_status != 0)); then cat "$test_dir/olwm.log" "$test_dir/xephyr.log" 2>/dev/null || true; fi
    kill ${wm_pid:-} ${xephyr_pid:-} ${host_pid:-} 2>/dev/null || true
    kill -KILL ${wm_pid:-} ${xephyr_pid:-} ${host_pid:-} 2>/dev/null || true
    wait ${wm_pid:-} ${xephyr_pid:-} ${host_pid:-} 2>/dev/null || true
    rm -rf "$test_dir"
}
trap cleanup EXIT
wait_display() {
    for ((i=0; i<100; i++)); do
        xdpyinfo -display "$1" >/dev/null 2>&1 && return
        sleep 0.05
    done
    return 1
}
gcc -std=c99 -D_DEFAULT_SOURCE -Wall -Wextra -Werror -Wno-unused-parameter -I"$OPENWINHOME/include" tests/resize-test.c -L"$OPENWINHOME/lib" -lxview -lolgx -lX11 -ldl -o "$test_dir/test"
Xvfb :99 -screen 0 1920x1080x24 >"$test_dir/host.log" 2>&1 &
host_pid=$!
wait_display :99
fonts="$OPENWINHOME/share/fonts/openlook/bdf"
DISPLAY=:99 Xephyr :98 -resizeable -screen 1280x800 -ac -nolisten tcp \
    -fp "$fonts/misc,$fonts/75dpi,$fonts/100dpi,built-ins" >"$test_dir/xephyr.log" 2>&1 &
xephyr_pid=$!
wait_display :98
export DISPLAY=:98
xrdb -merge config/Xdefaults
"$OPENWINHOME/bin/olwm" >"$test_dir/olwm.log" 2>&1 &
wm_pid=$!
for ((i=0;i<100;i++)); do
    xprop -root _SUN_WM_PROTOCOLS 2>/dev/null | grep -q _SUN_OL_WIN_ATTR_5 && break
    sleep 0.05
done
timeout 30 "$test_dir/test"
kill -0 "$wm_pid"
