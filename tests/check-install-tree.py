#!/usr/bin/env python3
"""Check the default Linux runtime package, including installed ELF paths."""
import os
from pathlib import Path
import subprocess
import sys


def check(root):
    root = root.resolve()
    prefix = root / "usr/local"
    assert {path.name for path in root.iterdir()} == {"etc", "usr"}
    assert {path.name for path in prefix.iterdir()} == {"bin", "lib", "share"}
    programs = (
        "olwm filemgr clock cmdtool shelltool textedit props xtoolplaces "
        "olwmslave disktool vwm xcalc mosaix openlook-session"
    ).split()
    for program in programs:
        path = prefix / "bin" / program
        assert path.is_file() and os.access(path, os.X_OK), path
    assert (prefix / "bin/shelltool").readlink() == Path("cmdtool")
    for config in ("openwin-menu", "openwin-menu-programs", "openwin-menu-demo",
                   "Olwm", "text_extras_menu"):
        assert (root / "etc/openlook" / config).is_file(), config
    for resource in (
        "lib/libxview.so", "lib/libolgx.so", "lib/mosaix/cef/libcef.so",
        "lib/mosaix/cef/icudtl.dat", "lib/mosaix/cef/resources.pak",
        "lib/mosaix/cef/v8_context_snapshot.bin", "lib/mosaix/cef/LICENSE.txt",
        "share/openlook/help/filemgr.info", "share/openlook/environment.sh",
        "share/openlook/icons/bw-theme/calculator.xpm", "share/xsessions/openlook.desktop",
        "share/man/man1/olwm.1", "share/man/man1/xcalc.1",
        "share/locale/C/props/basic_setting", "share/terminfo/s/sun-cmd",
    ):
        assert (prefix / resource).is_file(), resource
    for leaf in ("misc", "75dpi", "100dpi"):
        directory = prefix / "share/fonts/openlook/bdf" / leaf
        assert (directory / "fonts.dir").is_file(), directory
        assert (directory / "fonts.alias").is_file(), directory
    assert list((prefix / "lib/mosaix/cef/locales").glob("*.pak"))

    for path in root.rglob("*"):
        if path.is_symlink():
            assert not path.readlink().is_absolute(), path
            assert path.resolve().is_relative_to(root) and path.exists(), path
        if not path.is_file():
            continue
        assert path.suffix not in {".a", ".o", ".c", ".h", ".cmake", ".log"}, path
        assert path.name not in {"Makefile", "CMakeCache.txt", "use-openwinhome.sh"}, path
        with path.open("rb") as stream:
            is_elf = stream.read(4) == b"\x7fELF"
        if not is_elf:
            continue
        dynamic = subprocess.check_output(["readelf", "-d", str(path)], text=True)
        for line in dynamic.splitlines():
            if "RPATH" in line or "RUNPATH" in line:
                assert "/build/" not in line and str(root) not in line, (path, line)
                assert all(part.startswith("$ORIGIN") for part in
                           line.split("[", 1)[1].split("]", 1)[0].split(":")), (path, line)
    print("Install tree passed: all applications, runtime resources, relative links, and ELF paths.")


if __name__ == "__main__":
    check(Path(sys.argv[1] if len(sys.argv) > 1 else "bin"))
