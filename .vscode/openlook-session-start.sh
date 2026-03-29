#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
DISPLAY_NUM=":2"
XE_PID_FILE="$ROOT_DIR/.vscode/.openlook-xephyr.pid"
WM_PID_FILE="$ROOT_DIR/.vscode/.openlook-olwm.pid"
WATCHDOG_PID_FILE="$ROOT_DIR/.vscode/.openlook-watchdog.pid"
LOG_DIR="$ROOT_DIR/.vscode"
FONT_BASE_STAGE="$ROOT_DIR/bin/share/fonts/openlook/bdf"
FONT_BASE_SOURCE="$ROOT_DIR/resources/fonts/bdf"
XDEFAULTS_FILE="$ROOT_DIR/config/Xdefaults"

mkdir -p "$LOG_DIR"

if ! command -v Xephyr >/dev/null 2>&1; then
  echo "Xephyr is required but not installed." >&2
  exit 1
fi

if ! command -v xdpyinfo >/dev/null 2>&1; then
  echo "xdpyinfo is required but not installed." >&2
  exit 1
fi

cleanup_pid_file_if_dead() {
  local pid_file="$1"
  if [ -f "$pid_file" ]; then
    local pid
    pid="$(cat "$pid_file" 2>/dev/null || true)"
    if [ -z "$pid" ] || ! kill -0 "$pid" 2>/dev/null; then
      rm -f "$pid_file"
    fi
  fi
}

cleanup_pid_file_if_dead "$XE_PID_FILE"
cleanup_pid_file_if_dead "$WM_PID_FILE"
cleanup_pid_file_if_dead "$WATCHDOG_PID_FILE"

pick_font_dir() {
  local leaf="$1"
  local candidate
  for base in "$FONT_BASE_STAGE" "$FONT_BASE_SOURCE"; do
    candidate="$base/$leaf"
    if [ -d "$candidate" ]; then
      printf '%s\n' "$candidate"
      return 0
    fi
  done
  return 1
}

FONT_DIR_MISC="$(pick_font_dir misc || true)"
FONT_DIR_75="$(pick_font_dir 75dpi || true)"
FONT_DIR_100="$(pick_font_dir 100dpi || true)"

apply_openlook_font_path() {
  if ! command -v xset >/dev/null 2>&1; then
    return 0
  fi

  add_fp_if_missing() {
    local fp="$1"
    [ -n "$fp" ] || return 0
    [ -d "$fp" ] || return 0
    if ! DISPLAY="$DISPLAY_NUM" xset q 2>/dev/null | grep -Fq "$fp"; then
      DISPLAY="$DISPLAY_NUM" xset +fp "$fp" || true
    fi
  }

  add_fp_if_missing "$FONT_DIR_MISC"
  add_fp_if_missing "$FONT_DIR_75"
  add_fp_if_missing "$FONT_DIR_100"
  DISPLAY="$DISPLAY_NUM" xset fp rehash || true
}

if ! xdpyinfo -display "$DISPLAY_NUM" >/dev/null 2>&1; then
  (
    setsid Xephyr "$DISPLAY_NUM" -screen 1280x800 -ac \
      >"$LOG_DIR/openlook-xephyr.log" 2>&1 < /dev/null &
    echo $! > "$XE_PID_FILE"
  )

  for _ in $(seq 1 40); do
    if xdpyinfo -display "$DISPLAY_NUM" >/dev/null 2>&1; then
      break
    fi
    sleep 0.1
  done
fi

if ! xdpyinfo -display "$DISPLAY_NUM" >/dev/null 2>&1; then
  echo "Failed to start Xephyr on $DISPLAY_NUM" >&2
  exit 1
fi

if command -v mkfontdir >/dev/null 2>&1; then
  [ -n "$FONT_DIR_MISC" ] && [ -d "$FONT_DIR_MISC" ] && (cd "$FONT_DIR_MISC" && mkfontdir >/dev/null 2>&1) || true
  [ -n "$FONT_DIR_75" ] && [ -d "$FONT_DIR_75" ] && (cd "$FONT_DIR_75" && mkfontdir >/dev/null 2>&1) || true
  [ -n "$FONT_DIR_100" ] && [ -d "$FONT_DIR_100" ] && (cd "$FONT_DIR_100" && mkfontdir >/dev/null 2>&1) || true
fi

apply_openlook_font_path

if [ -f "$XDEFAULTS_FILE" ] && command -v xrdb >/dev/null 2>&1; then
  DISPLAY="$DISPLAY_NUM" xrdb -merge "$XDEFAULTS_FILE" || true
fi

if [ ! -f "$WM_PID_FILE" ]; then
  (
    OPENWINHOME="$ROOT_DIR/bin" \
    XENVIRONMENT="$XDEFAULTS_FILE" \
    XV_USE_XGETVISUALINFO_ALL="1" \
    PATH="$ROOT_DIR/bin/bin:${PATH}" \
    LD_LIBRARY_PATH="$ROOT_DIR/bin/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}" \
    DISPLAY="$DISPLAY_NUM" \
      setsid "$ROOT_DIR/bin/bin/olwm" >"$LOG_DIR/openlook-olwm.log" 2>&1 < /dev/null &
    echo $! > "$WM_PID_FILE"
  )

  # Wait for olwm to stay alive and advertise OPEN LOOK WM protocols.
  # Without this handshake, debug launches can race and map client windows
  # before olwm owns redirection, resulting in undecorated top-left windows.
  wm_ready=0
  for _ in $(seq 1 120); do
    if ! kill -0 "$(cat "$WM_PID_FILE")" 2>/dev/null; then
      echo "olwm failed to start; see $LOG_DIR/openlook-olwm.log" >&2
      exit 1
    fi
    if DISPLAY="$DISPLAY_NUM" xprop -root _SUN_WM_PROTOCOLS 2>/dev/null | grep -q "_SUN_OL_WIN_ATTR_5"; then
      wm_ready=1
      break
    fi
    sleep 0.1
  done

  if [ "$wm_ready" -ne 1 ]; then
    echo "olwm did not become ready on $DISPLAY_NUM; see $LOG_DIR/openlook-olwm.log" >&2
    exit 1
  fi
fi

# Give olwm a brief grace period to finish initialization before debug target
# maps its first top-level window.
sleep "${OPENLOOK_WM_GRACE_SECONDS:-4}"

# Some WM/session startup paths reset X font path; enforce OpenLook font
# directories again right before launching debug targets.
apply_openlook_font_path

# Start a watchdog that terminates workspace client apps on this display if
# olwm exits unexpectedly. This prevents cppdbg sessions from hanging when the
# WM is closed manually.
if [ ! -f "$WATCHDOG_PID_FILE" ]; then
  (
    setsid "$ROOT_DIR/.vscode/openlook-wm-watchdog.sh" \
      "$DISPLAY_NUM" \
      "$ROOT_DIR" \
      "$WM_PID_FILE" \
      "$XE_PID_FILE" \
      "$WATCHDOG_PID_FILE" \
      "$LOG_DIR/openlook-watchdog.log" \
      >> "$LOG_DIR/openlook-watchdog.log" 2>&1 < /dev/null &
    echo $! > "$WATCHDOG_PID_FILE"
  )
fi

echo "OpenLook debug session ready on DISPLAY=$DISPLAY_NUM"

# Optional: keep this task alive (for VS Code background preLaunch task mode)
# so the task host does not reap process trees shortly after startup.
if [ "${OPENLOOK_TASK_HOLD:-0}" = "1" ]; then
  while true; do
    xe_alive=0
    wm_alive=0
    if [ -f "$XE_PID_FILE" ] && kill -0 "$(cat "$XE_PID_FILE" 2>/dev/null || true)" 2>/dev/null; then
      xe_alive=1
    fi
    if [ -f "$WM_PID_FILE" ] && kill -0 "$(cat "$WM_PID_FILE" 2>/dev/null || true)" 2>/dev/null; then
      wm_alive=1
    fi
    if [ "$xe_alive" -eq 0 ] && [ "$wm_alive" -eq 0 ]; then
      break
    fi
    sleep 1
  done
fi
