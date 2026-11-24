#!/usr/bin/env bash
# Exercise the real manager and XView overview in an isolated nested X server.
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR"
if [[ "${1:-}" != --inside ]]; then
    exec docker run --rm --init --platform linux/amd64 \
        --user "$(id -u):$(id -g)" \
        --mount "type=bind,src=$ROOT_DIR,dst=$ROOT_DIR" \
        --workdir "$ROOT_DIR" --entrypoint /bin/bash openlook-dev:local \
        "$ROOT_DIR/tests/run-virtual-desktop-smoke.sh" --inside
fi
test_dir="$(mktemp -d /tmp/openlook-desktops.XXXXXX)"
export HOME="$test_dir/home"
export OPENWINHOME="$ROOT_DIR/bin"
export LD_LIBRARY_PATH="$OPENWINHOME/lib"
export XENVIRONMENT="$ROOT_DIR/config/Xdefaults"
export XV_USE_XGETVISUALINFO_ALL=1
export DISPLAY=:98
log_dir="$ROOT_DIR/build/docker/vwm-smoke"
mkdir -p "$HOME" "$log_dir"
cleanup() {
    result=$?
    trap - EXIT
    if ((result)); then tail -n 30 "$log_dir"/*.log; fi
    for pid in ${overview_pid:-} ${clock_pid:-} ${editor_pid:-} ${wm_pid:-} \
               ${xephyr_pid:-} ${xvfb_pid:-}; do
        kill "$pid" 2>/dev/null || true
        wait "$pid" 2>/dev/null || true
    done
    rm -rf "$test_dir"
    exit "$result"
}
trap cleanup EXIT
wait_display() {
    for ((i=0; i<100; ++i)); do
        xdpyinfo -display "$1" >/dev/null 2>&1 && return
        sleep 0.05
    done
    return 1
}
gcc -std=c99 -Wall -Wextra -Werror tests/virtual-desktop-test.c \
    -lX11 -o "$test_dir/backend-test"
gcc -std=c99 -Wall -Wextra -Werror tests/mosaix-window-close.c \
    -lX11 -o "$test_dir/close-window"
Xvfb :99 -screen 0 1600x1000x24 >"$log_dir/xvfb.log" 2>&1 &
xvfb_pid=$!
wait_display :99
fonts="$OPENWINHOME/share/fonts/openlook/bdf"
DISPLAY=:99 Xephyr :98 -resizeable -screen 1100x760 -ac -nolisten tcp \
    -fp "$fonts/misc,$fonts/75dpi,$fonts/100dpi,built-ins" \
    >"$log_dir/xephyr.log" 2>&1 &
xephyr_pid=$!
wait_display :98
xrdb -merge config/Xdefaults
timeout 30 "$test_dir/backend-test" >"$log_dir/backend.log" 2>&1
cat "$log_dir/backend.log"
"$OPENWINHOME/bin/olwm" >"$log_dir/olwm.log" 2>&1 &
wm_pid=$!
for ((i=0; i<100; ++i)); do
    xprop -root _OPENLOOK_DESKTOP_VERSION 2>/dev/null | grep -q '= 1' && break
    sleep 0.05
done
"$OPENWINHOME/bin/vwm" >"$log_dir/overview.log" 2>&1 &
overview_pid=$!
"$OPENWINHOME/bin/clock" -Wp 40 200 >"$log_dir/clock.log" 2>&1 &
clock_pid=$!
"$OPENWINHOME/bin/textedit" -Wp 300 260 >"$log_dir/textedit.log" 2>&1 &
editor_pid=$!
sleep 0.5
timeout 30 python3 tests/vwm-ui-test.py >"$log_dir/ui.log" 2>&1
cat "$log_dir/ui.log"
overview=$(xdotool search --name '^Virtual Desktops$' | head -1)
xwd -root -silent | convert xwd:- "$log_dir/desktop.png"
"$test_dir/close-window" "$overview"
for ((i=0; i<100; ++i)); do
    kill -0 "$overview_pid" 2>/dev/null || break
    sleep 0.05
done
if kill -0 "$overview_pid" 2>/dev/null; then
    echo 'Overview did not exit after WM_DELETE_WINDOW.' >&2
    exit 1
fi
wait "$overview_pid"
overview_pid=""
kill -0 "$wm_pid"
echo 'Virtual desktop backend, native overview and normal shutdown passed.'
