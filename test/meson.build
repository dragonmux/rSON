crunch = find_program('crunch')
#crunchpp = find_program('crunch++')
#libcrunchpp = dependency('crunch++')
libcrunch = dependency('crunch')

commandExtra = [
	'-std=c++17',
	'-lcrunch',
	'-I@0@'.format(rSONSrcDir),
	substrate.get_variable('compile_args'),
	substrate.get_variable('link_args'),
]
if get_option('b_coverage')
	commandExtra += ['--coverage']
endif
if get_option('debug')
	commandExtra += ['--debug']
endif
if cxx.get_id() != 'msvc'
	commandExtra += ['-z', 'defs']
endif
if get_option('b_lto')
	commandExtra += ['-flto']
endif

if sanitizer != 'none'
	commandExtra += ['-fsanitize=' + sanitizer]
endif

rSONReaderTests = [
	'testJSONNull', 'testJSONBool', 'testJSONInt', 'testJSONFloat',
	'testJSONString', 'testJSONObject', 'testJSONArray', 'testParser',
]
rSONGeneralTests = [
	'testJSONErrors', 'testWriter', 'testHeader', #'testSocket',
	'testStream'
]
rSONTests = rSONReaderTests + rSONGeneralTests

rSONObjs = rSON.extract_objects(
	'JSONAtom.cpp', 'JSONErrors.cpp', 'String.cpp', 'Writer.cpp',
	'Stream.cpp', 'JSONNull.cpp', 'JSONBool.cpp', 'JSONInt.cpp',
	'JSONFloat.cpp', 'JSONString.cpp', 'JSONObject.cpp', 'JSONArray.cpp'
)

testSrcs = [
	'rSON.cpp', 'stream.cpp'
]

testHelpers = static_library(
	'testHelpers',
	testSrcs,
	pic: true,
	dependencies: [libcrunch, substrate],
	include_directories: include_directories('..'),
	install: false,
	build_by_default: true
)

foreach test : rSONReaderTests
	objects = [rSONObjs]
	if test == 'testParser'
		objects += rSON.extract_objects('Parser.cpp')
	endif

	custom_target(
		test,
		command: [crunchMake, '-s', '@INPUT@', '-o', '@OUTPUT@'] + commandExtra,
		input: [test + '.cpp', objects],
		output: test + '.so',
		build_by_default: true
	)
endforeach

custom_target(
	'testJSONErrors',
	command: [crunchMake, '-s', '@INPUT@', '-o', '@OUTPUT@'] + commandExtra,
	input: ['testJSONErrors.cpp', rSON.extract_objects('JSONErrors.cpp')],
	output: 'testJSONErrors.so',
	build_by_default: true
)

custom_target(
	'testWriter',
#	command: [crunchMake, '-s', '@INPUT@', '-o', '@OUTPUT@', rSON.full_path()] + commandExtra,
	command: [crunchMake, '-s', '@INPUT@', '-o', '@OUTPUT@'] + commandExtra,
#	input: ['testWriter.cpp'],
	input: ['testWriter.cpp', rSONObjs, rSON.extract_objects('Parser.cpp')],
	output: 'testWriter.so',
#	depends: rSON,
	build_by_default: true
)

custom_target(
	'testHeader',
	command: [crunchMake, '-s', '@INPUT@', '-o', '@OUTPUT@'] + commandExtra,
	input: ['testHeader.cpp', objects, testHelpers.extract_objects('rSON.cpp')],
	output: 'testHeader.so',
	build_by_default: true
)

custom_target(
	'testStream',
	command: [crunchMake, '-s', '-std=c++17', '@INPUT@', '-o', '@OUTPUT@', '-I' + rSONSrcDir],
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
