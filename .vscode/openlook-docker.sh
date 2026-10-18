#!/usr/bin/env bash
# Build and debug with container tools; only Docker and Bash run on the host.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
IMAGE="wischner/gcc-x86_64-linux-x11:latest"
CONTAINER="openlook-debug-$(printf '%s' "$ROOT_DIR" | cksum | cut -d ' ' -f 1)"
STAGE="$ROOT_DIR/bin"
BUILD="$ROOT_DIR/build/docker"

build_inside() {
  cmake -S "$ROOT_DIR" -B "$BUILD" -DCMAKE_BUILD_TYPE=Debug \
    -DOPENLOOK_STAGE_DIR="$STAGE"
  cmake --build "$BUILD" --parallel "$(nproc)"
}

case "${1:-}" in
  build-inside)
    build_inside
    ;;
  session-inside)
    build_inside
    fonts="$STAGE/share/fonts/openlook/bdf"
    DISPLAY="$OPENLOOK_PARENT_DISPLAY" Xephyr :2 -screen 1280x800 \
      -nolisten tcp -ac -fp "$fonts/misc,$fonts/75dpi,$fonts/100dpi,built-ins" \
      >"$BUILD/xephyr.log" 2>&1 < /dev/null &
    xephyr_pid=$!
    for ((attempt=0; attempt<100; attempt++)); do
      if xdpyinfo -display :2 >/dev/null 2>&1; then
        DISPLAY=:2 xrdb -merge "$ROOT_DIR/config/Xdefaults"
        echo 'Docker OpenLook display ready; debugger will launch olwm.'
        exit 0
      fi
      if ! kill -0 "$xephyr_pid" 2>/dev/null; then
        cat "$BUILD/xephyr.log" >&2
        exit 1
      fi
      sleep 0.1
    done
    echo "Xephyr did not become ready; see $BUILD/xephyr.log" >&2
    exit 1
    ;;
  build)
    exec docker run --rm --platform linux/amd64 \
      --user "$(id -u):$(id -g)" --env HOME=/tmp \
      --mount "type=bind,src=$ROOT_DIR,dst=$ROOT_DIR" --workdir "$ROOT_DIR" \
      --entrypoint /bin/bash "$IMAGE" "$0" build-inside
    ;;
  prepare)
    : "${DISPLAY:?An X11/XWayland desktop DISPLAY is required for Xephyr.}"
    if [[ ! -d /tmp/.X11-unix ]]; then
      echo 'The host X11 socket directory /tmp/.X11-unix is missing.' >&2
      exit 1
    fi
    case "$DISPLAY" in
      :*|unix:*) parent_number="${DISPLAY##*:}"; parent_number="${parent_number%%.*}" ;;
      *) echo 'Docker debugging requires a local X11/XWayland DISPLAY.' >&2; exit 1 ;;
    esac
    parent_socket="/tmp/.X11-unix/X$parent_number"
    if [[ ! -S "$parent_socket" ]]; then
      echo "Host X11 socket missing: $parent_socket" >&2
      exit 1
    fi
    auth_args=()
    auth_file="${XAUTHORITY:-$HOME/.Xauthority}"
    if [[ -f "$auth_file" ]]; then
      auth_args=(--mount "type=bind,src=$auth_file,dst=/tmp/openlook.xauthority,readonly"
        --env XAUTHORITY=/tmp/openlook.xauthority)
    fi
    # Remove only the dedicated debug container for this checkout.
    docker rm -f "$CONTAINER" >/dev/null 2>&1 || true
    docker run --detach --rm --platform linux/amd64 --name "$CONTAINER" \
      --user "$(id -u):$(id -g)" --cap-add SYS_PTRACE \
      --env HOME=/tmp --env DISPLAY=:2 --env OPENLOOK_PARENT_DISPLAY="$DISPLAY" \
      --env OPENWINHOME="$STAGE" --env LD_LIBRARY_PATH="$STAGE/lib" \
      --env XENVIRONMENT="$ROOT_DIR/config/Xdefaults" \
      --env XV_USE_XGETVISUALINFO_ALL=1 \
      --env PATH="$STAGE/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin" \
      --mount "type=bind,src=$ROOT_DIR,dst=$ROOT_DIR" \
      --tmpfs /tmp/.X11-unix:rw,mode=1777 \
      --mount "type=bind,src=$parent_socket,dst=$parent_socket,readonly" \
      "${auth_args[@]}" --workdir "$ROOT_DIR" \
      --entrypoint /bin/bash "$IMAGE" -c 'exec sleep infinity' >/dev/null
    trap 'docker rm -f "$CONTAINER" >/dev/null 2>&1 || true' ERR
    docker exec "$CONTAINER" /bin/bash "$0" session-inside
    ;;
  gdb)
    shift
    # cppdbg passes its debugger command and flags as one shell command.
    # Interpret that command inside the container, keeping MI stdin intact.
    if [[ "$#" -eq 1 ]]; then
      exec docker exec -i "$CONTAINER" /bin/sh -c "exec $1"
    fi
    exec docker exec -i "$CONTAINER" "$@"
    ;;
  stop)
    docker rm -f "$CONTAINER" >/dev/null 2>&1 || true
    ;;
  *)
    echo "Usage: $0 {build|prepare|gdb|stop}" >&2
    exit 2
    ;;
esac
