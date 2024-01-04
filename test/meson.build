# SPDX-License-Identifier: LGPL-3.0-or-later
# SPDX-FileCopyrightText: 2018-2024 Rachel Mant <git@dragonmux.network>
# SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

crunchProject = subproject(
	'crunch',
	default_options: [
		'cpp_std=@0@'.format(get_option('cpp_std')),
	],
)

crunchMake = find_program('crunchMake')
crunch = find_program('crunch')
libcrunch = dependency('crunch')
#crunchpp = crunchProject.get_variable('crunchpp')
libcrunchpp = dependency('crunch++')

command = [
	crunchMake,
	'-s',
	'@INPUT@',
	'-o', '@OUTPUT@',
	'-std=c++17',
	'-L@0@'.format(crunchProject.get_variable('libCrunchppPath')),
	'-L@0@'.format(crunchProject.get_variable('libCrunchPath')),
	'-lcrunch',
	f'-I@rSONIncludePath@',
	'-DrSON_EXPORT_API',
	'-D_FILE_OFFSET_BITS=64',
	libcrunch.get_variable('compile_args'),
	libcrunch.get_variable('link_args'),
	#libcrunchpp.get_variable('compile_args'),
	libcrunchpp.get_variable('link_args'),
	substrate.get_variable('compile_args'),
	substrate.get_variable('link_args'),
]

if get_option('b_coverage')
	command += ['--coverage']
endif
if get_option('debug')
	command += ['--debug']
endif
if not isWindows and cxx.get_linker_id() != 'ld64' and noUndefined
	command += ['-z', 'defs']
endif
if get_option('b_lto')
	command += ['-flto']
endif

if sanitizer != 'none'
	command += ['-fsanitize=' + sanitizer]
endif

if isWindows
	testExt = '.dll'
else
	testExt = '.so'
endif

rSONReaderTests = [
	'testJSONNull', 'testJSONBool', 'testJSONInt', 'testJSONFloat',
	'testJSONString', 'testJSONObject', 'testJSONArray', 'testParser',
]
rSONGeneralTests = [
	'testJSONErrors', 'testWriter', 'testHeader', 'testSocket',
	'testStream'
]
rSONTests = rSONReaderTests + rSONGeneralTests

rSONObjs = rSON.extract_objects(
	'jsonAtom.cxx', 'jsonErrors.cxx', 'string.cxx', 'writer.cxx',
	'stream.cxx', 'jsonNull.cxx', 'jsonBool.cxx', 'jsonInt.cxx',
	'jsonFloat.cxx', 'jsonString.cxx', 'jsonObject.cxx', 'jsonArray.cxx'
)

testSrcs = [
	'rSON.cpp', 'stream.cpp'
]

testHelpers = static_library(
	'testHelpers',
	testSrcs,
	cpp_args: ['-DrSON_EXPORT_API'],
	pic: true,
	dependencies: [libcrunch, substrate],
	include_directories: rSONIncludeDir,
	install: false,
	build_by_default: true
)

foreach test : rSONReaderTests
	objects = [rSONObjs]
	if test == 'testParser'
		objects += rSON.extract_objects('parser.cxx')
	endif

	custom_target(
		test,
		command: command,
		input: [test + '.cpp', objects],
		output: test + '.so',
		build_by_default: true
	)
endforeach

custom_target(
	'testJSONErrors',
	command: command,
	input: ['testJSONErrors.cpp', rSONObjs],
	output: 'testJSONErrors.so',
	build_by_default: true
)

custom_target(
	'testWriter',
	command: command,
	input: ['testWriter.cpp', rSONObjs, rSON.extract_objects('parser.cxx')],
	output: 'testWriter.so',
	build_by_default: true
)

custom_target(
	'testHeader',
	command: command,
	input: ['testHeader.cpp', objects, testHelpers.extract_objects('rSON.cpp')],
	output: 'testHeader.so',
	build_by_default: true
)

custom_target(
	'testStream',
	command: command,
	input: ['testStream.cpp', objects, testHelpers.extract_objects('stream.cpp')],
	output: 'testStream.so',
	build_by_default: true
)

foreach test : rSONTests
	test(
		test,
		crunch,
		args: test,
		workdir: meson.current_build_dir()
	)
endforeach
