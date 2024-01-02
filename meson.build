# SPDX-License-Identifier: LGPL-3.0-or-later
# SPDX-FileCopyrightText: 2018-2024 Rachel Mant <git@dragonmux.network>
# SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

project(
	'rSON',
	['c', 'cpp'],
	default_options: [
		'cpp_std=c++17',
		'warning_level=3',
		'b_ndebug=if-release',
		'b_pch=false'
	],
	version: '0.4.1',
	meson_version: '>=0.58',
	subproject_dir: 'deps'
)

cxx = meson.get_compiler('cpp')
sanitizer = get_option('b_sanitize')
noUndefined = get_option('b_lundef')
isWindows = target_machine.system() == 'windows'
pkgconfig = import('pkgconfig')

if host_machine.system() != 'windows' and host_machine.system() != 'darwin'
	add_project_link_arguments('-z', 'defs', language: 'cpp')
elif host_machine.system() != 'darwin'
	add_project_arguments('-DWIN32', '-D_WINDOWS', language: 'cpp')
	add_project_arguments('-D_CRT_SECURE_NO_WARNINGS', language: 'cpp')
	if cxx.get_id() == 'msvc'
		add_global_arguments('-wd4996', '-wd4800', '-wd4459', '-wd5030', language: ['c', 'cpp'])
	endif
endif

subdir('include')
subdir('src')

if get_option('build_tests')
	subdir('test')
endif
