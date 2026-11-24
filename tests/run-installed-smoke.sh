#!/usr/bin/env bash
# Install only bin/ into a disposable container: no source/build runtime is visible.
set -euo pipefail
if [[ "${1:-}" != --inside ]]; then
    root_dir="$(cd "$(dirname "$0")/.." && pwd)"
    mkdir -p "$root_dir/build/installed-smoke"
    exec docker run --rm --init --network none \
        --mount "type=bind,src=$root_dir/bin,dst=/package,readonly" \
        --mount "type=bind,src=$root_dir/tests,dst=/tests,readonly" \
        --mount "type=bind,src=$root_dir/build/installed-smoke,dst=/logs" \
        --entrypoint /bin/bash openlook-dev:local \
        /tests/run-installed-smoke.sh --inside
fi

cp -a /package/. /
unset OPENWINHOME OPENLOOK_CONFIG_DIR LD_LIBRARY_PATH XENVIRONMENT
export DISPLAY=:99
export XV_USE_XGETVISUALINFO_ALL=1
export MOSAIX_DISABLE_SANDBOX=1
ulimit -c 0
pids=()
cleanup() {
    for ((i=${#pids[@]}-1; i>=0; --i)); do
        kill "${pids[i]}" 2>/dev/null || true
        wait "${pids[i]}" 2>/dev/null || true
    done
}
trap cleanup EXIT
Xvfb :99 -screen 0 1280x900x24 > /logs/xvfb.log 2>&1 &
pids+=("$!")
for ((i=0; i<100; ++i)); do
    if xdpyinfo >/dev/null 2>&1; then break; fi
    sleep 0.05
done
xdpyinfo >/dev/null
/usr/local/bin/openlook-session > /logs/olwm.log 2>&1 &
pids+=("$!")
wm_pid=$!
sleep 0.5
kill -0 "$wm_pid"

for app in filemgr clock cmdtool shelltool textedit props disktool vwm xcalc mosaix; do
    args=()
    [[ "$app" != filemgr ]] || args=(-C)
    [[ "$app" != disktool ]] || args=(/)
    [[ "$app" != mosaix ]] || args=(about:blank)
    /usr/local/bin/"$app" "${args[@]}" > "/logs/$app.log" 2>&1 &
    pids+=("$!")
    [[ "$app" != xcalc ]] || calc_pid=$!
done
sleep 3
for pid in "${pids[@]}"; do kill -0 "$pid"; done

# The same real key/selection checks must work with installed libraries and fonts.
gcc /tests/read-primary-selection.c -lX11 -o /logs/read-selection
calc_window=$(xdotool search --name '^Calculator$' | tail -1)
xdotool windowmove "$calc_window" 30 30 windowraise "$calc_window"
timeout 40 python3 /tests/calculator-ui-test.py /logs > /logs/ui.log 2>&1 || {
    xwininfo -root -tree > /logs/window-tree.log 2>&1 || true
    xprop -id "$calc_window" WM_STATE WM_HINTS >> /logs/window-tree.log 2>&1 || true
    xwd -root -silent | convert xwd:- /logs/failure.png || true
    cat /logs/ui.log
    exit 1
}
cat /logs/ui.log

# Session utilities must resolve through the installation, too.
/usr/local/bin/xtoolplaces -f /tmp/openlook-session-layout
test -s /tmp/openlook-session-layout
test -n "$(xdotool search --name MosaiX)"
gcc /tests/mosaix-window-close.c -lX11 -o /logs/close-window
calc_window=$(xdotool search --name '^Calculator$' | tail -1)
/logs/close-window "$calc_window"
for ((i=0; i<60; ++i)); do
    kill -0 "$calc_pid" 2>/dev/null || break
    sleep 0.05
done
if kill -0 "$calc_pid" 2>/dev/null; then
    echo 'Installed Calculator did not close normally.' >&2
    exit 1
fi
wait "$calc_pid"
kill -0 "$wm_pid"
printf 'Installed-only launch, calculator interaction, session tools, and shutdown passed.\n'
