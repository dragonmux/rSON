project(
	'rSON',
	['c', 'cpp'],
	default_options: [
		'cpp_std=c++14',
		'warning_level=3',
		'b_pch=false'
	],
	version: '0.3.1',
	meson_version: '>=0.52'
)

cxx = meson.get_compiler('cpp')
pkgconfig = import('pkgconfig')
rSONSocketLibs = []

if target_machine.system() != 'windows'
	add_project_link_arguments('-z', 'defs', language: 'cpp')
else
	add_project_arguments('-DWIN32', '-D_WINDOWS', language: 'cpp')
	add_project_arguments('-D_CRT_SECURE_NO_WARNINGS', language: 'cpp')
	add_global_arguments('-wd4996', '-wd4800', '-wd4459', '-wd5030', language: ['c', 'cpp'])
	rSONSocketLibs += cxx.find_library('Ws2_32')
endif

add_project_arguments('-D__rSON__', language: 'cpp')
substrate = include_directories('substrate')

rSONSrc = [
	'JSONErrors.cpp', 'JSONAtom.cpp', 'JSONNull.cpp', 'JSONBool.cpp',
	'JSONInt.cpp', 'JSONFloat.cpp', 'JSONString.cpp', 'JSONObject.cpp',
	'JSONArray.cpp', 'String.cpp', 'Stream.cpp', 'Parser.cpp',
	'Writer.cpp'
]

rSONSocketSrc = ['rSON_socket.cpp']
rSONSrcDir = meson.current_source_dir()

rSON = library(
	'rSON',
	rSONSrc,
	include_directories: substrate,
	version: meson.project_version(),
	install: true
)

rSONSocket = library(
	'rSON.socket',
	rSONSocketSrc,
	link_with: [rSON],
	include_directories: substrate,
	dependencies: rSONSocketLibs,
	version: meson.project_version(),
	install: true
)

pkgconfig.generate(
	rSON,
	name: 'rSON',
	filebase: 'rSON',
	version: meson.project_version(),
	description: 'A simple but effective JSON parser for C++'
)

pkgconfig.generate(
	rSONSocket,
	name: 'rSON Socket',
	filebase: 'rSON_socket',
	libraries: [rSON],
	version: meson.project_version(),
	description: 'Sockets handling for rSON, a simple but effective JSON parser for C++'
)

install_headers(
	'rSON.h', 'rSON_socket.h'
)

crunchMake = find_program('crunchMake', required: false)
subdir('test', if_found: crunchMake)
