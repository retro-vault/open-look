#!/usr/bin/env bash
# Exercise arithmetic and real native controls in an isolated nested X server.
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR"
if [[ "${1:-}" != --inside ]]; then
    exec docker run --rm --init --platform linux/amd64 \
        --user "$(id -u):$(id -g)" \
        --mount "type=bind,src=$ROOT_DIR,dst=$ROOT_DIR" \
        --workdir "$ROOT_DIR" --entrypoint /bin/bash openlook-dev:local \
        "$ROOT_DIR/tests/run-calculator-smoke.sh" --inside
fi
export OPENWINHOME="$PWD/build/docker/runtime"
export OPENLOOK_CONFIG_DIR="$OPENWINHOME/etc/openlook"
export LD_LIBRARY_PATH="$OPENWINHOME/lib"
export XENVIRONMENT="$PWD/config/Xdefaults"
export XV_USE_XGETVISUALINFO_ALL=1
export DISPLAY=:98
log_dir="$PWD/build/docker/calculator-smoke"
mkdir -p "$log_dir"
ulimit -c 0
cleanup() {
    for pid in ${calculator_pid:-} ${wm_pid:-} ${xephyr_pid:-} ${xvfb_pid:-}; do
        kill "$pid" 2>/dev/null || true
        wait "$pid" 2>/dev/null || true
    done
}
trap cleanup EXIT
wait_display() {
    for ((i=0; i<100; ++i)); do
        xdpyinfo -display "$1" >/dev/null 2>&1 && return
        sleep 0.05
    done
    return 1
}
Xvfb :99 -screen 0 1000x750x24 >"$log_dir/xvfb.log" 2>&1 &
xvfb_pid=$!
wait_display :99
fonts="$OPENWINHOME/share/fonts/openlook/bdf"
DISPLAY=:99 Xephyr :98 -screen 800x600 -ac -nolisten tcp \
    -fp "$fonts/misc,$fonts/75dpi,$fonts/100dpi,built-ins" >"$log_dir/xephyr.log" 2>&1 &
xephyr_pid=$!
wait_display :98
xrdb -merge config/Xdefaults
"$OPENWINHOME/bin/olwm" >"$log_dir/olwm.log" 2>&1 &
wm_pid=$!
sleep 0.3
"$OPENWINHOME/bin/xcalc" -Wp 30 30 >"$log_dir/calculator.log" 2>&1 &
calculator_pid=$!
sleep 0.5
gcc -std=c99 -Wall -Wextra -Werror -Iapps/xcalc tests/calculator-test.c \
    apps/xcalc/math.c apps/xcalc/actions.c apps/xcalc/word.c -lm -o "$log_dir/engine-test"
"$log_dir/engine-test"
gcc -std=c99 -Wall -Wextra -Werror tests/read-primary-selection.c -lX11 \
    -o "$log_dir/read-selection"
timeout 25 python3 tests/calculator-ui-test.py "$log_dir" >"$log_dir/ui.log" 2>&1 || {
    cat "$log_dir/ui.log"; exit 1;
}
cat "$log_dir/ui.log"
calc_window=$(xdotool search --name '^Calculator$' | tail -1)
gcc -std=c99 -Wall -Wextra -Werror tests/mosaix-window-close.c -lX11 \
    -o "$log_dir/close-window"
"$log_dir/close-window" "$calc_window"
for ((i=0; i<60; ++i)); do
    kill -0 "$calculator_pid" 2>/dev/null || break
    sleep 0.05
done
if kill -0 "$calculator_pid" 2>/dev/null; then
    echo 'Calculator did not close normally.' >&2
    exit 1
fi
wait "$calculator_pid"
calculator_pid=""
kill -0 "$wm_pid"
printf 'Calculator normal shutdown passed.\n'
