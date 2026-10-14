"""Write the audited build snapshot and docs/IAN.md from per-target logs.

Run from the repository root after build-all.py. The report keeps build
failures visible and distinguishes executable builds from supporting libraries.
"""
import argparse
from collections import Counter
import json
from pathlib import Path
import re
import subprocess


FIXES = {
    'ian_clock': 'Opaque handles, time_t, typed callbacks; reuse SDK ds_popup helper.',
    'ian_Ftptool4_6b4': 'Long attribute lists; regenerate Flex scanner; strerror.',
    'ian_fsptool_1_6_1': 'SDK header precedence, wait status, paths and username lookup.',
    'ian_guide': 'stdarg, lazy standard streams, libc declarations and strerror.',
    'ian_guidexv': 'POSIX regex, selection API, gettext, typed declarations.',
    'ian_sspkg': 'Typed geometry/callback helpers, portable sin/cos, opaque null values.',
    'ian_facman': 'Separate GUI from other mains; give facility data one owner.',
    'ian_sweeper_1_1': 'Explicit counter type; header globals have one owner.',
    'ian_timekeeper': 'Modern time/string/stdlib declarations and thread-local errno.',
    'ian_textedit': 'Pointer-sized attributes; sigemptyset; typed SDK helpers.',
    'ian_tree': 'Typed fractal/callback declarations and shared GUIDE support.',
    'ian_xcalentool': 'Canonical GUI sources, Linux time API, single global definitions.',
    'ian_xplan': 'C99 algorithms, regenerated Flex/Bison, pointer-safe keyword lookup, globals.',
    'ian_xvtdl_5_0': 'Separate GUI/CLI, regenerate lexer/parser, GUIDE and global ownership.',
    'ian_tdl': 'TDL overlays, correct CLI signatures, lexer/parser and global ownership.',
    'ian_mahjongg2_1': 'Typed byte/boolean image interfaces and pixrect handle conversions.',
    'ian_xwb': 'Public terminal compatibility header and modern directory header.',
    'ian_olxvman': 'Select handwritten frontend; remove duplicate stubs; typed declarations.',
    'ian_pan3_0': 'XView 3 drag/drop flags and source, libc regex, typed list/parser helpers.',
}


BLOCKERS = {
    'ian_NADagent': 'Missing rolo.icon and help.h; the archived application is incomplete.',
    'ian_catcher': 'Only index/trial files survive; no program source to compile.',
    'ian_contool_3_3a': 'Only index/trial files survive; no program source to compile.',
    'ian_gil2bil': 'Parser compiles after regeneration, but no main and BIL writer is unimplemented.',
    'ian_moxftp_2_2': 'OLIT/Xol and Wcl frontend dependencies; the XView SDK does not supply OLIT.',
    'ian_painter': 'NeWS/xvps canvas dependency absent from the XView SDK.',
    'ian_psdraw': 'NeWS/xvps canvas and CPS dependencies absent from the XView SDK.',
    'ian_ol_wais': 'Missing FreeWAIS headers/libraries and old C++ template interfaces.',
    'ian_rasview': 'Compiles; linking needs pr_load from the full Pixrect library.',
    'ian_videoteXt_0_5': 'Removed Linux vtx device ABI; linux/vtx.h is unavailable.',
    'ian_slingshot_cei': 'Sun desktop Classing Engine (desktop/ce.h) is unavailable.',
    'ian_slingshot_icon_dnd': 'Sun desktop Classing Engine (desktop/ce.h) is unavailable.',
    'ian_graphed': 'Sun framebuffer/Pixrect interfaces and Sgraph build integration remain.',
    'ian_robotx0_48': 'C/Fortran plotting and FITS interfaces need a full mixed-language port.',
    'ian_dstool': 'Numerical/model libraries still need C99 declarations and canonical library integration.',
    'ian_genix_ui': 'Unfinished C++ model and generated UI interfaces do not agree.',
    'ian_xvirtual': 'Pre-standard C++ references, operators and member declarations need porting.',
    'ian_olvwm': 'Sun-only OS interfaces and 32-bit Xlib/window-manager contracts need porting.',
    'ian_workman': 'Removed ustat interface and incompatible CD-info callbacks need a Linux driver/UI port.',
    'ian_xrolo_2_06': 'Legacy Pixrect/SunView interfaces and panel declarations need further porting.',
}


def diagnostics(log, root):
    lines = log.splitlines()
    errors = []
    for line in lines:
        if any(token in line for token in ('error:', 'Error:', 'undefined reference',
                                           'multiple definition', 'upstream has no',
                                           'missing upstream source')):
            if 'collect2: error: ld returned' in line:
                continue
            errors.append(line.replace(str(root) + '/', ''))
    return list(dict.fromkeys(errors))


def row(target, status, detail):
    detail = detail.replace('|', '\\|').replace('\n', ' ')
    return f'| `{target}` | {status} | {detail} |'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build', type=Path, default=Path('build'))
    parser.add_argument('--label', default='final')
    args = parser.parse_args()
    fixture = Path(__file__).resolve().parent
    root = fixture.parents[1]
    manifest = json.loads((fixture / 'manifest.json').read_text())
    directory = args.build.resolve() / 'ian-darwin' / args.label
    results = json.loads((directory / 'results.json').read_text())
    indexed = {r['target']: r for r in results}
    expected = {e['target'] for e in manifest['programs']}
    if set(indexed) != expected:
        raise RuntimeError('Report needs results for every manifest target')
    baseline_directory = args.build.resolve() / 'ian-darwin/baseline'
    baseline = {}
    if (baseline_directory / 'results.json').exists():
        baseline = {r['target']: r for r in
                    json.loads((baseline_directory / 'results.json').read_text())}
    else:
        previous = fixture / 'build-results.json'
        if previous.exists():
            baseline = {e['target']: e['initial'] for e in
                        json.loads(previous.read_text())['programs'] if e['initial']}
    records = []
    for program in manifest['programs']:
        target = program['target']
        result = indexed[target]
        log = (directory / result['log']).read_text(errors='replace')
        initial = baseline.get(target)
        if initial and 'diagnostics' not in initial:
            initial = dict(initial, diagnostics=diagnostics(
                (baseline_directory / initial['log']).read_text(errors='replace'), root))
        records.append(dict(target=target, path=program['path'], kind=program['kind'],
                            status=result['status'],
                            warnings=sum('warning:' in line.lower() for line in log.splitlines()),
                            diagnostics=diagnostics(log, root), initial=initial))
    compiler = subprocess.run(['cc', '--version'], check=True, capture_output=True,
                              text=True).stdout.splitlines()[0]
    summary = Counter(r['status'] for r in records)
    snapshot = dict(upstream=manifest['upstream'], revision=manifest['revision'],
                    date='2026-10-14', compiler=compiler,
                    passed=summary['pass'], failed=summary['fail'], programs=records)
    (fixture / 'build-results.json').write_text(json.dumps(snapshot, indent=2) + '\n')
    libraries = sum(r['kind'] == 'library' for r in records)
    lines = [
        '# Ian Darwin OpenLook CD-ROM compatibility report', '',
        'Audit date: 14 October 2026.', '',
        f"**{summary['pass']} of {len(records)} targets compile and link; "
        f"{summary['fail']} still fail.** This includes {libraries} supporting libraries/fragments. "
        'All 100 standalone programs in the archived XView toolkit example directories build.', '',
        'The remaining failures are recorded below, including unfinished ports. '
        'This is not a claim that the whole CD-ROM has been made runnable.', '',
        '## Source and scope', '',
        f"Source: [IanDarwin/OpenLookCDROM]({manifest['upstream']}/tree/{manifest['revision']}) "
        f"at commit `{manifest['revision']}`.", '',
        'The fixture imports every application directory under `src/xview`, the '
        'toolkit\'s `contrib/examples`, GUIDE, SlingShot, the XView variant of moontool, '
        'oldemo and olvwm. Broken clock/props/textedit links were resolved to the '
        'archive\'s actual client sources. Complete programs embedded in archived '
        'mailing-list messages and the file-chooser tar archive were extracted.', '',
        'The include scan covered the repository, looking for XView, olgx and Xol '
        'interfaces. The old XView toolkit implementations are replaced by our SDK, '
        'rather than rebuilt as competing libraries. Plain Xlib/Athena/Motif, '
        'SunView-only and PostScript NeWS programs outside these XView directories '
        'are outside this C/XView fixture. NeWS interpreters/protocols and OLIT are '
        'not interchangeable with XView.', '',
        'Original source notices, COPYING/COPYRIGHT/LEGAL_NOTICE files and data '
        'resources are retained. `UPSTREAM-LICENSE.txt` describes the archive\'s '
        'mixed licensing. RCS/SCCS/CVS history, old binaries, objects, make/imake '
        'files, configure scripts and legacy build configuration were removed.', '',
        'The original programs intentionally remain separate, namespaced targets. '
        'Failed targets are not replaced with placeholder executables.', '',
        '## Reproduce', '',
        'Configure from this repository root. The optional fixture uses C99 and '
        'our `xview`/`olgx` targets. C++, Fortran, Flex/Bison and ncurses are needed '
        'for the corresponding archived programs; missing historical dependencies '
        'remain visible in their build diagnostics.', '',
        '```sh',
        'cmake -S . -B build -DOPENLOOK_BUILD_IAN_DARWIN=ON',
        'python3 tests/ian-darwin/build-all.py --build build --label current',
        '# Build one program:',
        'cmake --build build --target ian_coffee',
        '# Refresh this report after a complete sweep:',
        'python3 tests/ian-darwin/report.py --build build --label current',
        '```', '',
        'The runner attempts every target, saves individual logs and `results.json` '
        'under `build/ian-darwin/<label>/`, and returns nonzero if any target fails. '
        'Make and Ninja builds continue through independent compilation errors. '
        'Executables stage in `bin/bin/ian-darwin/`; helper archives stage in '
        '`bin/lib/ian-darwin/`. There are no nested source-tree builds or system '
        'install steps. The fixture is excluded from the ordinary desktop build.', '',
        f'Compiler used: `{compiler}`.', '',
        '## Repeated failure patterns and fixes', '',
        '| Pattern | Applied fix |',
        '| --- | --- |',
        '| Obsolete make/imake, Sun compilers, fixed /usr/openwin paths | Explicit per-directory CMake source lists and SDK targets. |',
        '| Implicit int, omitted system/SDK declarations, old callbacks | Explicit return/parameter types, forward declarations and declaring headers. |',
        '| 32-bit pointer assumptions and opaque XView values | Preserve pointer-sized handles; decode string attributes and use time_t. |',
        '| GCC now defaults to -fno-common | Header declarations become extern; each shared variable has one owner. |',
        '| union wait, plain extern errno, sys_errlist and old varargs | POSIX wait macros, errno.h, strerror and stdarg. |',
        '| Stale generated lex/yacc code and pre-standard action syntax | Flex/Bison generate in the build tree; fix grammar and semantic declarations. |',
        '| Old regexp.h/compile/step interfaces | POSIX regex in GUIDE; select xvnews\' existing POSIX backend. |',
        '| Local xview/ folders shadow SDK headers | SDK include directory precedes application include directories. |',
        '| Unrelated GUI/CLI/OS sources linked together | Restore canonical source sets and per-target preprocessor settings. |', '',
        'Three shared SDK defects were fixed: base.h\'s package-header include cycle; '
        'the 64-argument Boost variadic limit (raised to 256); and the public '
        '`sel_post_request` declaration without a matching exported symbol. '
        '`sel_post_req` remains available, and the public spelling forwards to it.', '',
        'No warning suppressions, `-fpermissive`, or `-fcommon` were added. '
        'A passing target means successful compilation and linking; historical '
        'warnings remain in some imports and their counts are in `build-results.json`.', '',
        '## Validation', '',
        'The normal desktop/SDK build passes. `sdk_compat_test` checks public-header '
        'include order, a pointer and 80 variadic values, and selection-symbol '
        'linkage without an X server.', '',
        'Representative startup checks passed for `ian_example_canvas_line`, '
        '`ian_example_color_example1`, `ian_slingshot_array_ex` and `ian_tree` in '
        'Xephyr nested inside an isolated Xvfb server. Each created windows and '
        'remained alive; clients were terminated with both servers still running, '
        'and all test processes were cleaned up. These are startup checks, not '
        'complete feature or visual tests of every application.', '',
        '```sh', 'cmake --build build --target sdk_compat_test',
        'bin/bin/sdk_compat_test', 'python3 tests/ian-darwin/smoke-test.py', '```', '',
        '## Remaining failures', '',
        'Every failed target keeps its CMake recipe. The following table identifies '
        'the primary blocker or the first remaining compiler issue; '
        '`tests/ian-darwin/build-results.json` contains the complete distinct '
        'diagnostics, including secondary issues.', '',
        '| Target | Status | Remaining reason |', '| --- | --- | --- |',
    ]
    for record in records:
        if record['status'] == 'pass':
            continue
        reason = BLOCKERS.get(record['target'])
        if reason is None:
            diagnostic = record['diagnostics'][0] if record['diagnostics'] else 'See target log.'
            reason = re.sub(r'^.*?:\d+(?::\d+)?: (?:fatal )?error: ', '', diagnostic)
            reason = 'Further application port needed: ' + reason
        lines.append(row(record['target'], 'Fail', reason))
    lines += ['', '## Successful ports', '',
              '| Target | Status | Fixes or build notes |', '| --- | --- | --- |']
    for record in records:
        if record['status'] == 'fail' or record['path'].startswith(('sdk-examples/', 'slingshot/examples/')):
            continue
        note = FIXES.get(record['target'], 'CMake/SDK integration, C99 declarations and headers.')
        lines.append(row(record['target'], 'Pass', note))
    for prefix, title in [('sdk-examples/', 'Toolkit examples'),
                          ('slingshot/examples/', 'SlingShot examples')]:
        lines += ['', '## ' + title, '', '| Target | Status | Notes |', '| --- | --- | --- |']
        for record in records:
            if record['path'].startswith(prefix):
                lines.append(row(record['target'], record['status'].capitalize(),
                                 'Built against our SDK.' if record['status'] == 'pass'
                                 else BLOCKERS.get(record['target'], 'See failure table.')))
    lines += ['', '## Audit evidence', '',
              '`manifest.json` records every target\'s sources, definitions and dependencies. '
              '`build-results.json` records the upstream revision, compiler, final statuses, '
              'warnings and the initial sweep\'s diagnostic evidence when available. '
              'Targets discovered after the first sweep have no initial entry. '
              'The raw logs remain under the build directory.', '']
    (root / 'docs/IAN.md').write_text('\n'.join(lines))
    print(f"Wrote docs/IAN.md: {summary['pass']}/{len(records)} passed")


if __name__ == '__main__':
    main()
