#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
XE_PID_FILE="$ROOT_DIR/.vscode/.openlook-xephyr.pid"
WM_PID_FILE="$ROOT_DIR/.vscode/.openlook-olwm.pid"
WATCHDOG_PID_FILE="$ROOT_DIR/.vscode/.openlook-watchdog.pid"

stop_from_pid_file() {
  local pid_file="$1"
  if [ -f "$pid_file" ]; then
    local pid
    pid="$(cat "$pid_file" 2>/dev/null || true)"
    if [ -n "$pid" ] && kill -0 "$pid" 2>/dev/null; then
      kill "$pid" 2>/dev/null || true
      sleep 0.1
      kill -9 "$pid" 2>/dev/null || true
    fi
    rm -f "$pid_file"
  fi
}

# Stop watchdog first, then WM, then nested X server.
stop_from_pid_file "$WATCHDOG_PID_FILE"
stop_from_pid_file "$WM_PID_FILE"
stop_from_pid_file "$XE_PID_FILE"

echo "OpenLook session stopped"
