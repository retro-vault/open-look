"""Attempt every archived CMake target and retain per-target diagnostics.

Configure first with -DOPENLOOK_BUILD_IAN_DARWIN=ON. All logs and results
are written under the selected build tree; no archived build tool is run.
"""
import argparse
from concurrent.futures import ThreadPoolExecutor
import json
from pathlib import Path
import subprocess
import time


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build', type=Path, default=Path('build'))
    parser.add_argument('--jobs', type=int, default=4)
    parser.add_argument('--label', default='current')
    parser.add_argument('--target', action='append', help='Limit to named targets')
    args = parser.parse_args()
    if args.jobs < 1:
        parser.error('--jobs must be positive')
    root = Path(__file__).resolve().parent
    manifest = json.loads((root / 'manifest.json').read_text())
    output = args.build.resolve() / 'ian-darwin' / args.label
    output.mkdir(parents=True, exist_ok=True)
    cache = (args.build / 'CMakeCache.txt').read_text()
    if 'CMAKE_GENERATOR:INTERNAL=Ninja' in cache:
        keep_going = ['--', '-k', '0']
    elif 'CMAKE_GENERATOR:INTERNAL=Unix Makefiles' in cache:
        keep_going = ['--', '-k']
    else:
        keep_going = []
    programs = [p for p in manifest['programs']
                if not args.target or p['target'] in args.target]
    # Build SDK dependencies once before parallel independent application builds.
    subprocess.run(['cmake', '--build', str(args.build), '--target', 'xview',
                    '-j', str(args.jobs)], check=True, stdout=subprocess.DEVNULL)

    def build(program):
        target = program['target']
        start = time.monotonic()
        with (output / (target + '.log')).open('w') as log:
            result = subprocess.run(
                ['cmake', '--build', str(args.build), '--target', target,
                 '--parallel', '1'] + keep_going,
                stdout=log, stderr=subprocess.STDOUT)
        entry = dict(target=target, path=program['path'],
                     status='pass' if result.returncode == 0 else 'fail',
                     seconds=round(time.monotonic() - start, 2),
                     log=target + '.log')
        print(f"{entry['status']:4} {target}", flush=True)
        return entry

    # Libraries first so clients do not race over common generated objects.
    libraries = [p for p in programs if p['kind'] == 'library']
    results = [build(p) for p in libraries]
    clients = [p for p in programs if p['kind'] != 'library']
    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        results.extend(pool.map(build, clients))
    (output / 'results.json').write_text(json.dumps(results, indent=2) + '\n')
    passed = sum(r['status'] == 'pass' for r in results)
    print(f'{passed}/{len(results)} targets passed. Results: {output}')
    return 0 if passed == len(results) else 1


if __name__ == '__main__':
    raise SystemExit(main())
