"""Audit an SDK-only installation without using checkout headers or libraries."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import os
from pathlib import Path
import shlex
import subprocess
import tempfile


def run(command, **kwargs):
    try:
        return subprocess.run(command, check=True, text=True, capture_output=True,
                              **kwargs)
    except subprocess.CalledProcessError as error:
        raise RuntimeError(f'{shlex.join(command)}\n{error.stderr}') from error


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build', default='build')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    build = Path(args.build).resolve()
    compiler = shlex.split(os.environ.get('CC', 'cc'))
    # Prevent ambient developer include/library paths masking install omissions.
    environment = dict(os.environ)
    for key in ('CPATH', 'C_INCLUDE_PATH', 'CPLUS_INCLUDE_PATH', 'LIBRARY_PATH',
                'LD_LIBRARY_PATH', 'PKG_CONFIG_PATH'):
        environment.pop(key, None)
    with tempfile.TemporaryDirectory(prefix='openlook-sdk-audit-') as temporary:
        prefix = Path(temporary)
        run(['cmake', '--install', str(build), '--prefix', str(prefix),
             '--component', 'SDK'], env=environment)
        source_headers = sorted((root / 'include').rglob('*.h'))
        for header in source_headers:
            relative = header.relative_to(root / 'include')
            installed = prefix / 'include' / relative
            if not installed.is_file() or installed.read_bytes() != header.read_bytes():
                raise RuntimeError(f'Missing or stale installed header: {relative}')
        for library in ('libxview.so', 'libxview.a', 'libolgx.so', 'libolgx.a'):
            if not (prefix / 'lib' / library).is_file():
                raise RuntimeError(f'Missing installed library: {library}')
        environment['PKG_CONFIG_PATH'] = str(prefix / 'lib/pkgconfig')
        flags = shlex.split(run(['pkg-config', '--cflags', 'xview'],
                                env=environment).stdout)
        libraries = shlex.split(run(['pkg-config', '--libs', 'xview'],
                                    env=environment).stdout)

        def preprocess(case):
            header, i18n = case
            relative = header.relative_to(root / 'include').as_posix()
            command = compiler + flags + (['-DOW_I18N'] if i18n else [])
            result = subprocess.run(command + ['-E', '-x', 'c', '-'],
                                    input=f'#include <{relative}>\n',
                                    cwd=prefix, env=environment, text=True,
                                    stdout=subprocess.DEVNULL,
                                    stderr=subprocess.PIPE)
            if result.returncode:
                return f'{relative} (OW_I18N={i18n}):\n{result.stderr}'
            return None

        cases = [(header, i18n) for header in source_headers
                 for i18n in (False, True)]
        with ThreadPoolExecutor(max_workers=4) as workers:
            failures = [failure for failure in workers.map(preprocess, cases)
                        if failure]
        if failures:
            raise RuntimeError('\n'.join(failures))
        # Preprocessing catches missing dependencies even in internal fragments
        # that are intentionally not standalone C translation units. Also compile
        # and link a real client using only the installed pkg-config interface.
        client = prefix / 'client.c'
        client.write_text('''#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/panel.h>
#include <xview/sel_svc.h>
#include <pixrect/pixrect_hs.h>
static Frame frame;
static void quit(Panel_item item, Event *event)
{ (void)item; (void)event; xv_destroy_safe(frame); }
int main(int argc, char **argv)
{
    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, 0);
    frame = xv_create(0, FRAME, FRAME_LABEL, "SDK test", 0);
    Panel panel = xv_create(frame, PANEL, 0);
    xv_create(panel, PANEL_BUTTON, PANEL_LABEL_STRING, "Quit",
              PANEL_NOTIFY_PROC, quit, 0);
    xv_main_loop(frame);
    return 0;
}
''')
        run(compiler + ['-std=c99'] + flags + [str(client)] + libraries +
            ['-o', str(prefix / 'client')], cwd=prefix, env=environment)
        static_flags = shlex.split(run(['pkg-config', '--static', '--libs', 'xview'],
                                      env=environment).stdout)
        archives = {'-lxview': str(prefix / 'lib/libxview.a'),
                    '-lolgx': str(prefix / 'lib/libolgx.a')}
        static_flags = [archives.get(flag, flag) for flag in static_flags]
        run(compiler + ['-std=c99'] + flags + [str(client)] + static_flags +
            ['-o', str(prefix / 'client-static')], cwd=prefix, env=environment)
        print(f'PASS: {len(source_headers)} installed headers, '
              f'{len(cases)} preprocessing checks, shared/static libraries, '
              'and standalone shared/static client compilation/linking')


if __name__ == '__main__':
    main()
