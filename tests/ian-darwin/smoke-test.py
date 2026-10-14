"""Check representative archived GUIs on an isolated nested Xephyr display.

An outer Xvfb supplies Xephyr's host display, so the user's desktop is not
needed. This is a startup check, not an interaction or rendering test.
"""
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import time


PROGRAMS = ('ian_example_canvas_line', 'ian_example_color_example1',
            'ian_slingshot_array_ex', 'ian_tree')


def stop(process):
    if process.poll() is None:
        process.terminate()
        try:
            process.wait(timeout=5)
        except subprocess.TimeoutExpired:
            process.kill()
            process.wait(timeout=5)


def start_server(command, directory, name, environment):
    display_file = (directory / (name + '.display')).open('w+')
    log = (directory / (name + '.log')).open('w+')
    server = subprocess.Popen(command + ['-displayfd', str(display_file.fileno())],
                              env=environment, pass_fds=(display_file.fileno(),),
                              stdout=log, stderr=log)
    try:
        for _ in range(100):
            display_file.seek(0)
            number = display_file.read().strip()
            if number:
                return server, ':' + number
            if server.poll() is not None:
                raise RuntimeError((directory / (name + '.log')).read_text())
            time.sleep(0.05)
        raise RuntimeError(f'{name} did not become ready')
    except BaseException:
        stop(server)
        raise
    finally:
        display_file.close()
        log.close()


def main():
    root = Path(__file__).resolve().parents[2]
    fonts = ','.join(str(root / 'bin/share/fonts/openlook/bdf' / name)
                     for name in ('misc', '75dpi', '100dpi'))
    fonts += ',/usr/share/fonts/X11/misc'
    for tool in ('Xvfb', 'Xephyr', 'xwininfo'):
        if not shutil.which(tool):
            raise RuntimeError(f'{tool} is required')
    with tempfile.TemporaryDirectory(prefix='openlook-ian-smoke-') as temporary:
        directory = Path(temporary)
        environment = dict(os.environ, HOME=str(directory),
                           OPENWINHOME=str(root / 'bin'),
                           LD_LIBRARY_PATH=str(root / 'bin/lib'),
                           XENVIRONMENT=str(root / 'config/Xdefaults'),
                           LANG='C', LC_ALL='C')
        outer = nested = None
        try:
            outer, host = start_server(
                ['Xvfb', '-screen', '0', '1024x768x24', '-nolisten', 'tcp',
                 '-ac', '-fp', fonts], directory, 'host', environment)
            environment['DISPLAY'] = host
            nested, display = start_server(
                ['Xephyr', '-screen', '1024x768', '-nolisten', 'tcp',
                 '-ac', '-fp', fonts], directory, 'nested', environment)
            environment['DISPLAY'] = display
            for name in PROGRAMS:
                executable = root / 'bin/bin/ian-darwin' / name
                with (directory / (name + '.log')).open('w') as log:
                    client = subprocess.Popen([str(executable)], env=environment,
                                              stdout=log, stderr=log)
                    try:
                        time.sleep(1)
                        if client.poll() is not None:
                            raise RuntimeError(f'{name} exited: ' +
                                               (directory / (name + '.log')).read_text())
                        tree = subprocess.run(['xwininfo', '-root', '-tree'],
                                              env=environment, check=True,
                                              capture_output=True, text=True)
                        if '0 children' in tree.stdout:
                            raise RuntimeError(f'{name} did not create a window')
                        print(f'PASS {name}', flush=True)
                    finally:
                        # Keep both servers alive while checking client shutdown.
                        stop(client)
        finally:
            if nested is not None:
                stop(nested)
            if outer is not None:
                stop(outer)


if __name__ == '__main__':
    main()
