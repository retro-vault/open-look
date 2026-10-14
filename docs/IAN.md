# Ian Darwin OpenLook CD-ROM compatibility report

Audit date: 14 October 2026.

**176 of 202 targets compile and link; 26 still fail.** This includes 4 supporting libraries/fragments. All 100 standalone programs in the archived XView toolkit example directories build.

The remaining failures are recorded below, including unfinished ports. This is not a claim that the whole CD-ROM has been made runnable.

## Source and scope

Source: [IanDarwin/OpenLookCDROM](https://github.com/IanDarwin/OpenLookCDROM/tree/2aa032ddc0a737bb420463c0a508a0f4867c1667) at commit `2aa032ddc0a737bb420463c0a508a0f4867c1667`.

The fixture imports every application directory under `src/xview`, the toolkit's `contrib/examples`, GUIDE, SlingShot, the XView variant of moontool, oldemo and olvwm. Broken clock/props/textedit links were resolved to the archive's actual client sources. Complete programs embedded in archived mailing-list messages and the file-chooser tar archive were extracted.

The include scan covered the repository, looking for XView, olgx and Xol interfaces. The old XView toolkit implementations are replaced by our SDK, rather than rebuilt as competing libraries. Plain Xlib/Athena/Motif, SunView-only and PostScript NeWS programs outside these XView directories are outside this C/XView fixture. NeWS interpreters/protocols and OLIT are not interchangeable with XView.

Original source notices, COPYING/COPYRIGHT/LEGAL_NOTICE files and data resources are retained. `UPSTREAM-LICENSE.txt` describes the archive's mixed licensing. RCS/SCCS/CVS history, old binaries, objects, make/imake files, configure scripts and legacy build configuration were removed.

The original programs intentionally remain separate, namespaced targets. Failed targets are not replaced with placeholder executables.

## Reproduce

Configure from this repository root. The optional fixture uses C99 and our `xview`/`olgx` targets. C++, Fortran, Flex/Bison and ncurses are needed for the corresponding archived programs; missing historical dependencies remain visible in their build diagnostics.

```sh
cmake -S . -B build -DOPENLOOK_BUILD_IAN_DARWIN=ON
python3 tests/ian-darwin/build-all.py --build build --label current
# Build one program:
cmake --build build --target ian_coffee
# Refresh this report after a complete sweep:
python3 tests/ian-darwin/report.py --build build --label current
```

The runner attempts every target, saves individual logs and `results.json` under `build/ian-darwin/<label>/`, and returns nonzero if any target fails. Make and Ninja builds continue through independent compilation errors. Executables stage in `bin/bin/ian-darwin/`; helper archives stage in `bin/lib/ian-darwin/`. There are no nested source-tree builds or system install steps. The fixture is excluded from the ordinary desktop build.

Compiler used: `cc (Ubuntu 16-20260315-1ubuntu1~24~ppa1) 16.0.1 20260315 (experimental) [trunk r16-8100-g3aca3bae8ee]`.

## Repeated failure patterns and fixes

| Pattern | Applied fix |
| --- | --- |
| Obsolete make/imake, Sun compilers, fixed /usr/openwin paths | Explicit per-directory CMake source lists and SDK targets. |
| Implicit int, omitted system/SDK declarations, old callbacks | Explicit return/parameter types, forward declarations and declaring headers. |
| 32-bit pointer assumptions and opaque XView values | Preserve pointer-sized handles; decode string attributes and use time_t. |
| GCC now defaults to -fno-common | Header declarations become extern; each shared variable has one owner. |
| union wait, plain extern errno, sys_errlist and old varargs | POSIX wait macros, errno.h, strerror and stdarg. |
| Stale generated lex/yacc code and pre-standard action syntax | Flex/Bison generate in the build tree; fix grammar and semantic declarations. |
| Old regexp.h/compile/step interfaces | POSIX regex in GUIDE; select xvnews' existing POSIX backend. |
| Local xview/ folders shadow SDK headers | SDK include directory precedes application include directories. |
| Unrelated GUI/CLI/OS sources linked together | Restore canonical source sets and per-target preprocessor settings. |

Three shared SDK defects were fixed: base.h's package-header include cycle; the 64-argument Boost variadic limit (raised to 256); and the public `sel_post_request` declaration without a matching exported symbol. `sel_post_req` remains available, and the public spelling forwards to it.

No warning suppressions, `-fpermissive`, or `-fcommon` were added. A passing target means successful compilation and linking; historical warnings remain in some imports and their counts are in `build-results.json`.

## Validation

The normal desktop/SDK build passes. `sdk_compat_test` checks public-header include order, a pointer and 80 variadic values, and selection-symbol linkage without an X server.

Representative startup checks passed for `ian_example_canvas_line`, `ian_example_color_example1`, `ian_slingshot_array_ex` and `ian_tree` in Xephyr nested inside an isolated Xvfb server. Each created windows and remained alive; clients were terminated with both servers still running, and all test processes were cleaned up. These are startup checks, not complete feature or visual tests of every application.

```sh
cmake --build build --target sdk_compat_test
bin/bin/sdk_compat_test
python3 tests/ian-darwin/smoke-test.py
```

## Remaining failures

Every failed target keeps its CMake recipe. The following table identifies the primary blocker or the first remaining compiler issue; `tests/ian-darwin/build-results.json` contains the complete distinct diagnostics, including secondary issues.

| Target | Status | Remaining reason |
| --- | --- | --- |
| `ian_NADagent` | Fail | Missing rolo.icon and help.h; the archived application is incomplete. |
| `ian_apex_1_1` | Fail | Further application port needed: passing argument 1 of ‘sprintf’ from incompatible pointer type [-Wincompatible-pointer-types] |
| `ian_catcher` | Fail | Only index/trial files survive; no program source to compile. |
| `ian_contool_3_3a` | Fail | Only index/trial files survive; no program source to compile. |
| `ian_dstool` | Fail | Numerical/model libraries still need C99 declarations and canonical library integration. |
| `ian_genix_ui` | Fail | Unfinished C++ model and generated UI interfaces do not agree. |
| `ian_gil2bil` | Fail | Parser compiles after regeneration, but no main and BIL writer is unimplemented. |
| `ian_graphed` | Fail | Sun framebuffer/Pixrect interfaces and Sgraph build integration remain. |
| `ian_moxftp_2_2` | Fail | OLIT/Xol and Wcl frontend dependencies; the XView SDK does not supply OLIT. |
| `ian_ol_wais` | Fail | Missing FreeWAIS headers/libraries and old C++ template interfaces. |
| `ian_owrolo` | Fail | Further application port needed: conflicting types for ‘init_selections’; have ‘int(Panel)’ {aka ‘int(long unsigned int)’} |
| `ian_painter` | Fail | NeWS/xvps canvas dependency absent from the XView SDK. |
| `ian_privtool_0_83` | Fail | Further application port needed: passing argument 4 of ‘qsort’ from incompatible pointer type [-Wincompatible-pointer-types] |
| `ian_psdraw` | Fail | NeWS/xvps canvas and CPS dependencies absent from the XView SDK. |
| `ian_rasview` | Fail | Compiles; linking needs pr_load from the full Pixrect library. |
| `ian_robotx0_48` | Fail | C/Fortran plotting and FITS interfaces need a full mixed-language port. |
| `ian_scantool` | Fail | Further application port needed: implicit declaration of function ‘get_options’ [-Wimplicit-function-declaration] |
| `ian_videoteXt_0_5` | Fail | Removed Linux vtx device ABI; linux/vtx.h is unavailable. |
| `ian_workman` | Fail | Removed ustat interface and incompatible CD-info callbacks need a Linux driver/UI port. |
| `ian_xrolo_2_06` | Fail | Legacy Pixrect/SunView interfaces and panel declarations need further porting. |
| `ian_xvgr_2_09` | Fail | Further application port needed: invalid use of incomplete typedef ‘Display’ |
| `ian_xvirtual` | Fail | Pre-standard C++ references, operators and member declarations need porting. |
| `ian_xvnews_2_3` | Fail | Further application port needed: implicit declaration of function ‘xv_usage’; did you mean ‘xvnews_usage’? [-Wimplicit-function-declaration] |
| `ian_olvwm` | Fail | Sun-only OS interfaces and 32-bit Xlib/window-manager contracts need porting. |
| `ian_slingshot_cei` | Fail | Sun desktop Classing Engine (desktop/ce.h) is unavailable. |
| `ian_slingshot_icon_dnd` | Fail | Sun desktop Classing Engine (desktop/ce.h) is unavailable. |

## Successful ports

| Target | Status | Fixes or build notes |
| --- | --- | --- |
| `ian_Ftptool4_6b4` | Pass | Long attribute lists; regenerate Flex scanner; strerror. |
| `ian_clock` | Pass | Opaque handles, time_t, typed callbacks; reuse SDK ds_popup helper. |
| `ian_cmdtool` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_coffee` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_disktool_2_0` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_facman` | Pass | Separate GUI from other mains; give facility data one owner. |
| `ian_faxtool1_2` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_fsptool_1_6_1` | Pass | SDK header precedence, wait status, paths and username lookup. |
| `ian_guide` | Pass | stdarg, lazy standard streams, libc declarations and strerror. |
| `ian_mahjongg2_1` | Pass | Typed byte/boolean image interfaces and pixrect handle conversions. |
| `ian_olxvman` | Pass | Select handwritten frontend; remove duplicate stubs; typed declarations. |
| `ian_pan3_0` | Pass | XView 3 drag/drop flags and source, libc regex, typed list/parser helpers. |
| `ian_phonetool` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_props` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_props_ui` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_sidtool` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_sweeper_1_1` | Pass | Explicit counter type; header globals have one owner. |
| `ian_textedit` | Pass | Pointer-sized attributes; sigemptyset; typed SDK helpers. |
| `ian_timekeeper` | Pass | Modern time/string/stdlib declarations and thread-local errno. |
| `ian_tree` | Pass | Typed fractal/callback declarations and shared GUIDE support. |
| `ian_xcalentool` | Pass | Canonical GUI sources, Linux time API, single global definitions. |
| `ian_xplan` | Pass | C99 algorithms, regenerated Flex/Bison, pointer-safe keyword lookup, globals. |
| `ian_xvmines_1_0` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_xvnotice` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_xvokscript` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_xvset_0_80` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_xvtdl_5_0` | Pass | Separate GUI/CLI, regenerate lexer/parser, GUIDE and global ownership. |
| `ian_xwb` | Pass | Public terminal compatibility header and modern directory header. |
| `ian_yamailt_ui` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_oldemo` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_moontool` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_xpm_image_demo` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_guidexv` | Pass | POSIX regex, selection API, gettext, typed declarations. |
| `ian_sspkg` | Pass | Typed geometry/callback helpers, portable sin/cos, opaque null values. |
| `ian_apex_animate` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_article_multi_frame` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_article_send_action` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_article_timers` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_article_notify_pipe` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_file_chooser_cmdrunner` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_file_chooser_filelist` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_file_chooser_new_load` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_file_chooser_pathfield` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_file_chooser_text_editor` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_file_chooser_text_exten` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_guide_tree` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_calendar_calencheck` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_calendar_cal2ct` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_calendar_month2ct` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_calendar_mt2ct` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_tkrefmt` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_tree_generated` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_timekeeper_alternate` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_rolo_test` | Pass | CMake/SDK integration, C99 declarations and headers. |
| `ian_tdl` | Pass | TDL overlays, correct CLI signatures, lexer/parser and global ownership. |

## Toolkit examples

| Target | Status | Notes |
| --- | --- | --- |
| `ian_example_canvas_canvas_event` | Pass | Built against our SDK. |
| `ian_example_canvas_canvas_input` | Pass | Built against our SDK. |
| `ian_example_canvas_line` | Pass | Built against our SDK. |
| `ian_example_canvas_scroll_view` | Pass | Built against our SDK. |
| `ian_example_canvas_split_views` | Pass | Built against our SDK. |
| `ian_example_color_animate` | Pass | Built against our SDK. |
| `ian_example_color_color` | Pass | Built against our SDK. |
| `ian_example_color_color_animate` | Pass | Built against our SDK. |
| `ian_example_color_color_logo` | Pass | Built against our SDK. |
| `ian_example_color_color_objs` | Pass | Built against our SDK. |
| `ian_example_color_color_panel` | Pass | Built against our SDK. |
| `ian_example_color_example1` | Pass | Built against our SDK. |
| `ian_example_color_example2` | Pass | Built against our SDK. |
| `ian_example_color_example3` | Pass | Built against our SDK. |
| `ian_example_color_example4` | Pass | Built against our SDK. |
| `ian_example_color_x_draw` | Pass | Built against our SDK. |
| `ian_example_color_animate2` | Pass | Built against our SDK. |
| `ian_example_cursor_hot_spot` | Pass | Built against our SDK. |
| `ian_example_cursor_simple_cursor` | Pass | Built against our SDK. |
| `ian_example_cursor_stop_cursor` | Pass | Built against our SDK. |
| `ian_example_defaults_default_size` | Pass | Built against our SDK. |
| `ian_example_defaults_default_text` | Pass | Built against our SDK. |
| `ian_example_dnd_dest` | Pass | Built against our SDK. |
| `ian_example_dnd_source1` | Pass | Built against our SDK. |
| `ian_example_dnd_source2` | Pass | Built against our SDK. |
| `ian_example_extensions_logo` | Pass | Built against our SDK. |
| `ian_example_extensions_bitmap` | Pass | Built against our SDK. |
| `ian_example_extensions_image` | Pass | Built against our SDK. |
| `ian_example_fonts_disp_fonts` | Pass | Built against our SDK. |
| `ian_example_fonts_fonts` | Pass | Built against our SDK. |
| `ian_example_fonts_simple_font` | Pass | Built against our SDK. |
| `ian_example_fonts_type_font` | Pass | Built against our SDK. |
| `ian_example_fonts_disp_fonts2` | Pass | Built against our SDK. |
| `ian_example_frames_command_frame` | Pass | Built against our SDK. |
| `ian_example_frames_done_proc` | Pass | Built against our SDK. |
| `ian_example_frames_frame_color` | Pass | Built against our SDK. |
| `ian_example_frames_hdrs_n_footers` | Pass | Built against our SDK. |
| `ian_example_frames_popup` | Pass | Built against our SDK. |
| `ian_example_frames_simple_frame` | Pass | Built against our SDK. |
| `ian_example_frames_subframe` | Pass | Built against our SDK. |
| `ian_example_icons_icon_demo` | Pass | Built against our SDK. |
| `ian_example_icons_icon_demo2` | Pass | Built against our SDK. |
| `ian_example_menus_menu` | Pass | Built against our SDK. |
| `ian_example_menus_menu_dir` | Pass | Built against our SDK. |
| `ian_example_menus_menu_dir2` | Pass | Built against our SDK. |
| `ian_example_menus_pin_menu` | Pass | Built against our SDK. |
| `ian_example_menus_simple_menu` | Pass | Built against our SDK. |
| `ian_example_menus_xv_menu` | Pass | Built against our SDK. |
| `ian_example_misc_fullscreen` | Pass | Built against our SDK. |
| `ian_example_misc_multi_display` | Pass | Built against our SDK. |
| `ian_example_misc_multiscreen` | Pass | Built against our SDK. |
| `ian_example_misc_screen` | Pass | Built against our SDK. |
| `ian_example_misc_svrimage` | Pass | Built against our SDK. |
| `ian_example_misc_vkbd_fonts` | Pass | Built against our SDK. |
| `ian_example_misc_vkbd_colors` | Pass | Built against our SDK. |
| `ian_example_misc_visual` | Pass | Built against our SDK. |
| `ian_example_notice_notice` | Pass | Built against our SDK. |
| `ian_example_notice_simple_notice` | Pass | Built against our SDK. |
| `ian_example_notice_trigger_notice` | Pass | Built against our SDK. |
| `ian_example_notifier_animate` | Pass | Built against our SDK. |
| `ian_example_notifier_interpose` | Pass | Built against our SDK. |
| `ian_example_notifier_notify` | Pass | Built against our SDK. |
| `ian_example_notifier_notify_input` | Pass | Built against our SDK. |
| `ian_example_notifier_ntfy_do_dis` | Pass | Built against our SDK. |
| `ian_example_notifier_ntfy_pipe` | Pass | Built against our SDK. |
| `ian_example_notifier_ntfy_sig` | Pass | Built against our SDK. |
| `ian_example_panels_btn_menu` | Pass | Built against our SDK. |
| `ian_example_panels_choices` | Pass | Built against our SDK. |
| `ian_example_panels_client_data` | Pass | Built against our SDK. |
| `ian_example_panels_item_move` | Pass | Built against our SDK. |
| `ian_example_panels_list_glyphs` | Pass | Built against our SDK. |
| `ian_example_panels_list_6_glyphs` | Pass | Built against our SDK. |
| `ian_example_panels_panel_repaint` | Pass | Built against our SDK. |
| `ian_example_panels_quit` | Pass | Built against our SDK. |
| `ian_example_panels_repaint` | Pass | Built against our SDK. |
| `ian_example_panels_simple_panel` | Pass | Built against our SDK. |
| `ian_example_panels_slider` | Pass | Built against our SDK. |
| `ian_example_panels_stop_frame` | Pass | Built against our SDK. |
| `ian_example_panels_panel_dnd` | Pass | Built against our SDK. |
| `ian_example_scrollbar_scroll_cells` | Pass | Built against our SDK. |
| `ian_example_scrollbar_scroll_cells2` | Pass | Built against our SDK. |
| `ian_example_scrollbar_scrollto` | Pass | Built against our SDK. |
| `ian_example_selection_sel_hold` | Pass | Built against our SDK. |
| `ian_example_selection_sel_req` | Pass | Built against our SDK. |
| `ian_example_seln_svc_long_seln` | Pass | Built against our SDK. |
| `ian_example_seln_svc_seln` | Pass | Built against our SDK. |
| `ian_example_seln_svc_seln_line` | Pass | Built against our SDK. |
| `ian_example_seln_svc_simple_seln` | Pass | Built against our SDK. |
| `ian_example_seln_svc_text_seln` | Pass | Built against our SDK. |
| `ian_example_sv_compat_pw_draw` | Pass | Built against our SDK. |
| `ian_example_textsw_sample_tty` | Pass | Built against our SDK. |
| `ian_example_textsw_textsw` | Pass | Built against our SDK. |
| `ian_example_textsw_textsw_font` | Pass | Built against our SDK. |
| `ian_example_textsw_textsw_to_ttysw` | Pass | Built against our SDK. |
| `ian_example_textsw_ttycurses` | Pass | Built against our SDK. |
| `ian_example_textsw_xv_termsw` | Pass | Built against our SDK. |
| `ian_example_ttysw_sample_tty` | Pass | Built against our SDK. |
| `ian_example_ttysw_textsw_to_ttysw` | Pass | Built against our SDK. |
| `ian_example_ttysw_ttycurses` | Pass | Built against our SDK. |
| `ian_example_ttysw_xv_termsw` | Pass | Built against our SDK. |

## SlingShot examples

| Target | Status | Notes |
| --- | --- | --- |
| `ian_slingshot_array_ex` | Pass | Built against our SDK. |
| `ian_slingshot_clockobj` | Pass | Built against our SDK. |
| `ian_slingshot_drop_ex` | Pass | Built against our SDK. |
| `ian_slingshot_drag_ex` | Pass | Built against our SDK. |
| `ian_slingshot_cei` | Fail | Sun desktop Classing Engine (desktop/ce.h) is unavailable. |
| `ian_slingshot_icon_dnd` | Fail | Sun desktop Classing Engine (desktop/ce.h) is unavailable. |
| `ian_slingshot_hist` | Pass | Built against our SDK. |
| `ian_slingshot_custom` | Pass | Built against our SDK. |
| `ian_slingshot_hist_stdin` | Pass | Built against our SDK. |
| `ian_slingshot_darea2` | Pass | Built against our SDK. |
| `ian_slingshot_darea1` | Pass | Built against our SDK. |
| `ian_slingshot_drawimage` | Pass | Built against our SDK. |
| `ian_slingshot_drawline` | Pass | Built against our SDK. |
| `ian_slingshot_drawtext` | Pass | Built against our SDK. |
| `ian_slingshot_puzzdemo` | Pass | Built against our SDK. |
| `ian_slingshot_nonlin` | Pass | Built against our SDK. |
| `ian_slingshot_temp_grip` | Pass | Built against our SDK. |
| `ian_slingshot_gripdemo` | Pass | Built against our SDK. |
| `ian_slingshot_drawrect` | Pass | Built against our SDK. |
| `ian_slingshot_example1` | Pass | Built against our SDK. |
| `ian_slingshot_select` | Pass | Built against our SDK. |
| `ian_slingshot_tacho` | Pass | Built against our SDK. |
| `ian_slingshot_tlstool` | Pass | Built against our SDK. |

## Audit evidence

`manifest.json` records every target's sources, definitions and dependencies. `build-results.json` records the upstream revision, compiler, final statuses, warnings and the initial sweep's diagnostic evidence when available. Targets discovered after the first sweep have no initial entry. The raw logs remain under the build directory.
