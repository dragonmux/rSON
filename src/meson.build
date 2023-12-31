# SPDX-License-Identifier: LGPL-3.0-or-later
# SPDX-FileCopyrightText: 2023 Rachel Mant <git@dragonmux.network>
# SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

substrate = subproject(
	'substrate',
	default_options: [
		'build_tests=false',
		'cpp_std=@0@'.format(get_option('cpp_std')),
	],
).get_variable(
	'substrate_dep'
)

add_project_arguments('-DrSON_EXPORT_API', language: 'cpp')

rSONSrc = [
	'jsonErrors.cxx', 'jsonAtom.cxx', 'jsonNull.cxx', 'jsonBool.cxx',
	'jsonInt.cxx', 'jsonFloat.cxx', 'jsonString.cxx', 'jsonObject.cxx',
	'jsonArray.cxx', 'string.cxx', 'stream.cxx', 'parser.cxx',
	'writer.cxx'
]

rSON = library(
	'rSON',
	rSONSrc,
	include_directories: rSONIncludeDir,
	dependencies: [substrate],
	gnu_symbol_visibility: 'inlineshidden',
	version: meson.project_version(),
	install: true
)

if not meson.is_subproject()
	pkgconfig.generate(
		rSON,
		name: 'rSON',
		filebase: 'rSON',
		version: meson.project_version(),
		description: 'A simple but effective JSON parser for C++'
	)
else
	rSON_dep = declare_dependency(
		include_directories: rSONIncludeDir,
		link_with: rSON,
		version: meson.project_version()
	)
endif

subdir('socket')
