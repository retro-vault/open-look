#!/usr/bin/env bash
# Verify menu-style startup, rendering, child exit, and normal browser shutdown.
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR"
if [[ "${1:-}" != "--inside" ]]; then
    exec docker run --rm --init --platform linux/amd64 \
        --user "$(id -u):$(id -g)" --env HOME=/tmp/mosaix-smoke-home \
        --env MOSAIX_DISABLE_SANDBOX=1 \
        --mount "type=bind,src=$ROOT_DIR,dst=$ROOT_DIR" \
        --workdir "$ROOT_DIR" --entrypoint /bin/bash openlook-dev:local \
        "$ROOT_DIR/tests/run-mosaix-smoke.sh" --inside
fi
export OPENWINHOME="$ROOT_DIR/bin"
export LD_LIBRARY_PATH="$OPENWINHOME/lib"
export XV_USE_XGETVISUALINFO_ALL=1
export XENVIRONMENT="$ROOT_DIR/config/Xdefaults"
export DISPLAY=:98
test_dir="$(mktemp -d /tmp/mosaix-smoke.XXXXXX)"
log_dir="$ROOT_DIR/build/docker/mosaix-smoke"
mkdir -p "$HOME" "$log_dir"
export XDG_CONFIG_HOME="$test_dir/config"
export XDG_DATA_HOME="$test_dir/data"
export XDG_STATE_HOME="$test_dir/state"
cleanup() {
    result=$?
    trap - EXIT
    if ((result)); then tail -n 40 "$log_dir"/*.log; fi
    for pid in ${browser_pid:-} ${watchdog:-} ${wm_pid:-} ${xephyr_pid:-} ${xvfb_pid:-}; do
        kill "$pid" 2>/dev/null || true
        wait "$pid" 2>/dev/null || true
    done
    rm -rf -- "$test_dir"
    exit "$result"
}
trap cleanup EXIT
gcc -std=c99 -Wall -Wextra -Werror tests/mosaix-window-close.c \
    -lX11 -o "$test_dir/window-close"
cp tests/data/mosaix-controls.html "$test_dir/page.html"
mkdir -p "$XDG_CONFIG_HOME/openlook/mosaix"
printf 'homepage=file://%s/page.html\n' "$test_dir" \
    >"$XDG_CONFIG_HOME/openlook/mosaix/preferences"
Xvfb :99 -screen 0 1280x800x24 >"$log_dir/xvfb.log" 2>&1 &
xvfb_pid=$!
for ((i=0;i<100;i++)); do
    xdpyinfo -display :99 >/dev/null 2>&1 && break
    sleep 0.1
done
fonts="$OPENWINHOME/share/fonts/openlook/bdf"
DISPLAY=:99 Xephyr :98 -screen 1280x800 -ac -nolisten tcp \
    -fp "$fonts/misc,$fonts/75dpi,$fonts/100dpi,built-ins" \
    >"$log_dir/xephyr.log" 2>&1 &
xephyr_pid=$!
for ((i=0;i<100;i++)); do
    xdpyinfo >/dev/null 2>&1 && break
    sleep 0.1
done
xdpyinfo >/dev/null
xrdb -merge config/Xdefaults
"$OPENWINHOME/bin/olwm" >"$log_dir/olwm.log" 2>&1 &
wm_pid=$!
for ((i=0;i<100;i++)); do
    xprop -root _SUN_WM_PROTOCOLS 2>/dev/null | grep -q _SUN_OL_WIN_ATTR_5 && break
    sleep 0.1
done
for close_mode in keyboard window_manager; do
    # No command-line workaround: use the same environment as the F5 menu.
    "$OPENWINHOME/bin/mosaix" >"$log_dir/$close_mode.log" 2>&1 &
    browser_pid=$!
    window=""
    for ((i=0;i<150;i++)); do
        window=$(xdotool search --name '^MosaiX smoke ready.* - MosaiX$' | head -1) && break
        kill -0 "$browser_pid"
        sleep 0.1
    done
    test -n "$window"
    # Let short-lived Chromium utility processes return before checking logs.
    sleep 3
    kill -0 "$browser_pid"
    if [[ "$close_mode" == keyboard ]]; then
        python3 tests/mosaix-layout-test.py "$window" "$test_dir/window-close"
        python3 tests/mosaix-controls-test.py "$window"
        xdotool mousemove --window "$window" 300 200 click 1 key ctrl+q
    else
        "$test_dir/window-close" "$window"
    fi
    ( sleep 15; kill "$browser_pid" 2>/dev/null || true ) &
    watchdog=$!
    wait "$browser_pid"
    browser_pid=""
    kill "$watchdog" 2>/dev/null || true
    wait "$watchdog" 2>/dev/null || true
    watchdog=""
    if grep -E 'stack smashing|FATAL|Error initializing NSS|symbol missing' \
            "$log_dir/$close_mode.log"; then
        exit 1
    fi
done
test -s "$XDG_DATA_HOME/pki/nssdb/cert9.db"
echo 'MosaiX startup, JavaScript rendering, relaunch, and both close paths passed.'
