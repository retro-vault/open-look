"""Run props palette regression checks against isolated Xvfb displays.

Build with: cmake --build build --target props_color_test
Run with: python3 tests/run-props-colors.py [--xvfb /path/to/Xvfb]
"""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import time


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--xvfb", default=shutil.which("Xvfb"))
    parser.add_argument("--build", type=Path, default=Path("build"))
    args = parser.parse_args()
    if not args.xvfb:
        parser.error("Xvfb is required; install it or pass --xvfb")
    root = Path(__file__).resolve().parents[1]
    executable = (args.build / "tests/props_color_test").resolve()
    fonts = ",".join(str(root / "bin/share/fonts/openlook/bdf" / d)
                     for d in ("misc", "75dpi", "100dpi"))
    fonts += ",/usr/share/fonts/X11/misc"
    for depth, visual in ((8, 3), (16, 4), (24, 4), (8, 0), (8, 1)):
        with tempfile.TemporaryDirectory(prefix="openlook-colors-") as directory:
            directory = Path(directory)
            with (directory / "display").open("w+") as display_file, \
                    (directory / "server.log").open("w+") as log:
                server = subprocess.Popen(
                    [args.xvfb, "-displayfd", str(display_file.fileno()),
                     "-screen", "0", f"1024x768x{depth}", "-cc", str(visual),
                     "-nolisten", "tcp", "-ac", "-fp", fonts],
                    pass_fds=(display_file.fileno(),), stdout=log, stderr=log)
                try:
                    display = ""
                    for _ in range(100):
                        display_file.seek(0)
                        display = display_file.read().strip()
                        if display:
                            break
                        if server.poll() is not None:
                            log.seek(0)
                            raise RuntimeError(log.read())
                        time.sleep(0.05)
                    if not display:
                        raise RuntimeError("Xvfb did not become ready")
                    env = dict(os.environ, DISPLAY=":" + display,
                               HOME=str(directory), LANG="C", LC_ALL="C",
                               OPENWINHOME=str(root / "bin"),
                               XENVIRONMENT=str(root / "config/Xdefaults"))
                    subprocess.run([str(executable)], env=env,
                                   check=True, timeout=30)
                finally:
                    server.terminate()
                    server.wait(timeout=5)


if __name__ == "__main__":
    main()
