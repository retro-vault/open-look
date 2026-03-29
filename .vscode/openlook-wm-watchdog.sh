#!/usr/bin/env bash
set -euo pipefail

if [ "$#" -lt 6 ]; then
  echo "usage: $0 <display> <root_dir> <wm_pid_file> <xe_pid_file> <watchdog_pid_file> <log_file>" >&2
  exit 2
fi

DISPLAY_NUM="$1"
ROOT_DIR="$2"
WM_PID_FILE="$3"
XE_PID_FILE="$4"
WATCHDOG_PID_FILE="$5"
LOG_FILE="$6"

log() {
  printf '[%s] %s\n' "$(date '+%Y-%m-%d %H:%M:%S')" "$*" >> "$LOG_FILE"
}

cleanup_self() {
  rm -f "$WATCHDOG_PID_FILE"
}
trap cleanup_self EXIT

is_pid_alive_and_matches() {
  local pid="$1"
  local expected_prefix="$2"
  local exe
  if [ -z "$pid" ] || ! kill -0 "$pid" 2>/dev/null; then
    return 1
  fi
  exe="$(readlink -f "/proc/$pid/exe" 2>/dev/null || true)"
  case "$exe" in
    "$expected_prefix"*) return 0 ;;
    *) return 1 ;;
  esac
}

process_has_display_env() {
  local pid="$1"
  grep -zqx "DISPLAY=$DISPLAY_NUM" "/proc/$pid/environ" 2>/dev/null
}

process_cwd_under_root() {
  local pid="$1"
  local cwd
  cwd="$(readlink -f "/proc/$pid/cwd" 2>/dev/null || true)"
  case "$cwd" in
    "$ROOT_DIR"*) return 0 ;;
    *) return 1 ;;
  esac
}

kill_debugger_ancestors_for_pid() {
  local pid="$1"
  local sig="$2"
  local cur ppid exe base

  cur="$pid"
  for _ in 1 2 3 4 5 6; do
    ppid="$(awk '{print $4}' "/proc/$cur/stat" 2>/dev/null || true)"
    if [ -z "$ppid" ] || [ "$ppid" -le 1 ]; then
      break
    fi

    exe="$(readlink -f "/proc/$ppid/exe" 2>/dev/null || true)"
    base="$(basename "$exe" 2>/dev/null || true)"
    if [ "$base" = "gdb" ] || [ "$base" = "gdbserver" ]; then
      log "matched debugger ancestor pid=$ppid for app_pid=$pid (SIG$sig)"
      kill "-$sig" "$ppid" 2>/dev/null || true
      break
    fi

    cur="$ppid"
  done
}

kill_debugger_fallback() {
  local sig="$1"
  local pid

  for pid in $(pgrep -x gdb 2>/dev/null || true); do
    if process_has_display_env "$pid" && process_cwd_under_root "$pid"; then
      log "matched fallback gdb pid=$pid (SIG$sig)"
      kill "-$sig" "$pid" 2>/dev/null || true
    fi
  done

  for pid in $(pgrep -x gdbserver 2>/dev/null || true); do
    if process_has_display_env "$pid" && process_cwd_under_root "$pid"; then
      log "matched fallback gdbserver pid=$pid (SIG$sig)"
      kill "-$sig" "$pid" 2>/dev/null || true
    fi
  done
}

kill_workspace_clients_on_display() {
  local sig="$1"
  local app pids
  for app in textedit hello clock cmdtool props; do
    pids="$(pgrep -x "$app" 2>/dev/null || true)"
    if [ -n "$pids" ]; then
      log "matched app=$app pids=$pids for SIG$sig"
      for pid in $pids; do
        kill_debugger_ancestors_for_pid "$pid" "$sig"
      done
    fi
    pkill "-$sig" -x "$app" >/dev/null 2>&1 || true
  done

  # If app already exited/crashed before we scan, still terminate lingering
  # debugger processes tied to this DISPLAY and workspace.
  kill_debugger_fallback "$sig"
}

log "watchdog started for DISPLAY=$DISPLAY_NUM"
log "watchdog PATH=$PATH"
if ! command -v pkill >/dev/null 2>&1; then
  log "pkill not found in PATH, watchdog exiting"
  exit 1
fi

proto_missing_count=0
while true; do
  WM_PID="$(cat "$WM_PID_FILE" 2>/dev/null || true)"
  XE_PID="$(cat "$XE_PID_FILE" 2>/dev/null || true)"

  wm_ok=0
  xe_ok=0
  proto_ok=0

  if is_pid_alive_and_matches "$WM_PID" "$ROOT_DIR/bin/bin/olwm"; then
    wm_ok=1
  fi
  if is_pid_alive_and_matches "$XE_PID" ""; then
    xe_ok=1
  fi
  if DISPLAY="$DISPLAY_NUM" xprop -root _SUN_WM_PROTOCOLS 2>/dev/null | grep -q "_SUN_OL_WIN_ATTR_5"; then
    proto_ok=1
    proto_missing_count=0
  else
    proto_missing_count=$((proto_missing_count + 1))
  fi

  if [ "$wm_ok" -eq 0 ] || [ "$xe_ok" -eq 0 ] || [ "$proto_missing_count" -ge 3 ]; then
    log "wm session lost (wm_ok=$wm_ok xe_ok=$xe_ok proto_ok=$proto_ok miss=$proto_missing_count), terminating workspace clients on $DISPLAY_NUM"
    kill_workspace_clients_on_display TERM
    sleep 0.25
    kill_workspace_clients_on_display KILL
    exit 0
  fi

  sleep 0.3
done
