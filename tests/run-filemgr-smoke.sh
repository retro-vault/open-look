#!/usr/bin/env bash
# Exercise the real File Manager in container Xephyr and a temporary home.
set -euo pipefail
ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT_DIR"
if [[ "${1:-}" != "--inside" ]]; then
    exec docker run --rm --platform linux/amd64 --cap-add SYS_PTRACE \
        --user "$(id -u):$(id -g)" \
        --mount "type=bind,src=$ROOT_DIR,dst=$ROOT_DIR" \
        --workdir "$ROOT_DIR" --entrypoint /bin/bash \
        wischner/gcc-x86_64-linux-x11:latest \
        "$ROOT_DIR/tests/run-filemgr-smoke.sh" --inside
fi
if [[ ! -x "$ROOT_DIR/bin/docker/bin/filemgr" ]]; then
    echo 'Build first: bash .vscode/openlook-docker.sh build' >&2
    exit 1
fi
export OPENWINHOME="$ROOT_DIR/bin/docker"
export LD_LIBRARY_PATH="$OPENWINHOME/lib"
export PATH="$OPENWINHOME/bin:$PATH"
test_dir="$(mktemp -d /tmp/filemgr-test.XXXXXX)"
mkdir "$test_dir/home"
mkdir "$test_dir/files"
gcc -std=c99 -D_DEFAULT_SOURCE -Wall -Wextra -Werror tests/filemgr-window-test.c -lX11 -o "$test_dir/window-test"
printf 'sample contents\n' >"$test_dir/files/sample.txt"
fonts="$OPENWINHOME/share/fonts/openlook/bdf"
Xvfb :99 -screen 0 1280x800x24 >/tmp/filemgr-test-xvfb.log 2>&1 &
xvfb_pid=$!
cleanup() {
    kill ${wm_pid:-} ${xephyr_pid:-} "$xvfb_pid" 2>/dev/null || true
    wait ${wm_pid:-} ${xephyr_pid:-} "$xvfb_pid" 2>/dev/null || true
    rm -rf -- "$test_dir"
}
trap cleanup EXIT
for ((i=0;i<100;i++)); do DISPLAY=:99 xdpyinfo >/dev/null 2>&1 && break; sleep 0.1; done
DISPLAY=:99 Xephyr :98 -screen 1280x800 -ac -nolisten tcp -fp "$fonts/misc,$fonts/75dpi,$fonts/100dpi,built-ins" >/tmp/filemgr-test-xephyr.log 2>&1 &
xephyr_pid=$!
export DISPLAY=:98
for ((i=0;i<100;i++)); do xdpyinfo >/dev/null 2>&1 && break; sleep 0.1; done
xrdb -merge config/Xdefaults
olwm >/tmp/filemgr-test-olwm.log 2>&1 &
wm_pid=$!
for ((i=0;i<100;i++)); do xprop -root _SUN_WM_PROTOCOLS 2>/dev/null | grep -q _SUN_OL_WIN_ATTR_5 && break; sleep 0.1; done
cat >"$test_dir/smoke.gdb" <<GDB
set pagination off
set confirm off
set disable-randomization off
break start_tool
run
python assert gdb.parse_and_eval('Fm->file[Fm->curr_wno]->num_objects') == 1
call new_file(1, "Created Folder")
python assert gdb.parse_and_eval('Fm->file[Fm->curr_wno]->num_objects') == 2
call stop_rename()
call new_file(0, "Created Document")
python assert gdb.parse_and_eval('Fm->file[Fm->curr_wno]->num_objects') == 3
call stop_rename()
set \$catch = (Catch)calloc(1, sizeof(struct Dnd_Receive_Struct))
set \$catch->dest_wno = Fm->curr_wno
call copy_by_filename("$test_dir/files/sample.txt", "$test_dir/files/Created Folder", \$catch, 0)
call do_move("$test_dir/files/Created Document", "$test_dir/files/Renamed Document")
call fm_openfile("$test_dir/files/Created Folder", 0, 1, Fm->curr_wno)
python assert gdb.parse_and_eval('(char *)Fm->file[Fm->curr_wno]->path').string() == '$test_dir/files/Created Folder'
set Fm->show_quit_prompt = 0
shell "$test_dir/window-test" >"$test_dir/window-test.log" 2>&1 &
continue
quit
GDB
HOME="$test_dir/home" timeout -s INT 25 gdb --batch -x "$test_dir/smoke.gdb" --args "$OPENWINHOME/bin/filemgr" -C -d "$test_dir/files"
test -d "$test_dir/files/Created Folder"
test -f "$test_dir/files/Renamed Document"
test ! -e "$test_dir/files/Created Document"
cmp "$test_dir/files/sample.txt" "$test_dir/files/Created Folder/sample.txt"
convert build/docker/filemgr-smoke.xwd build/docker/filemgr-smoke.png
cat "$test_dir/window-test.log"
echo 'File Manager creation, copy, rename, navigation, and WM close passed.'
