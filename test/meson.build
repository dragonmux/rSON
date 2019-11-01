crunchMake = find_program('crunchMake', required: false)
if crunchMake.found()
	crunch = find_program('crunch')
	crunchpp = find_program('crunch++')

	rSONReaderTests = [
		'testJSONNull', 'testJSONBool', 'testJSONInt', 'testJSONFloat',
		'testJSONString', 'testJSONObject', 'testJSONArray', 'testParser',
	]
	rSONGeneralTests = [
		'testJSONErrors', 'testWriter'#, 'testHeader', 'testSocket'
	]
	rSONTests = rSONReaderTests + rSONGeneralTests

	rSONObjs = rSON.extract_objects(
		'JSONAtom.cpp', 'JSONErrors.cpp', 'String.cpp', 'Writer.cpp',
		'Stream.cpp', 'JSONNull.cpp', 'JSONBool.cpp', 'JSONInt.cpp',
		'JSONFloat.cpp', 'JSONString.cpp', 'JSONObject.cpp', 'JSONArray.cpp'
	)

	foreach test : rSONReaderTests
		objects = [rSONObjs]
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

	custom_target(
		'testJSONErrors',
		command: [crunchMake, '-s', '@INPUT@', '-o', '@OUTPUT@', '-I' + rSONSrcDir],
		input: ['testJSONErrors.cpp', rSON.extract_objects('JSONErrors.cpp')],
		output: 'testJSONErrors.so',
		build_by_default: true
	)

	custom_target(
		'testWriter',
	#	command: [crunchMake, '-s', '@INPUT@', '-o', '@OUTPUT@', '-I' + rSONSrcDir, rSON.full_path()],
		command: [crunchMake, '-s', '@INPUT@', '-o', '@OUTPUT@', '-I' + rSONSrcDir],
	#	input: ['testWriter.cpp'],
		input: ['testWriter.cpp', rSONObjs, rSON.extract_objects('Parser.cpp')],
		output: 'testWriter.so',
	#	depends: rSON,
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
endif
