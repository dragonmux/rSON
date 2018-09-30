rSONReaderTests = [
	'testJSONNull', 'testJSONBool', 'testJSONInt', 'testJSONFloat',
	'testJSONString', 'testJSONObject', 'testJSONArray', 'testParser'
]
rSONGeneralTests = [
	'testJSONErrors', 'testWriter', 'testHeader', 'testSocket'
]
rSONTests = rSONReaderTests + rSONGeneralTests

crunchMake = find_program('crunchMake')
crunchpp = find_program('crunch++')

rSONObjs = rSON.extract_objects(
	'JSONAtom.cpp', 'JSONErrors.cpp', 'String.cpp', 'Writer.cpp',
	'Stream.cpp', 'JSONNull.cpp', 'JSONBool.cpp', 'JSONInt.cpp',
	'JSONFloat.cpp', 'JSONString.cpp', 'JSONObject.cpp', 'JSONArray.cpp'
)

foreach test : rSONReaderTests
	objects = rSONObjs
	if test == 'testParser'
		objects += rSON.extract_objects('Parser.cpp')
	endif

	custom_target(
		test,
		command: [crunchMake, '-s', '@INPUT@', '-o', '@OUTPUT@', '-I' + rSONSrcDir],
		input: [test + '.cpp', objects],
		output: test + '.so',
		build_by_default: true
	)
endforeach

test(
	'rSON',
	crunchpp,
	args: rSONTests,
	workdir: meson.current_build_dir()
)
