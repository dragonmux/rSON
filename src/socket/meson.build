# SPDX-License-Identifier: LGPL-3.0-or-later
# SPDX-FileCopyrightText: 2023-2024 Rachel Mant <git@dragonmux.network>
# SPDX-FileContributor: Written by Rachel Mant <git@dragonmux.network>

rSONSocketLibs = []
if host_machine.system() == 'windows'
	rSONSocketLibs += cxx.find_library('Ws2_32')
endif

rSONSocketSrc = ['socket.cxx']

rSONSocket = library(
	'rSON.socket',
	rSONSocketSrc,
	cpp_args: ['-DrSON_SOCKET_EXPORT_API'],
	include_directories: [rSONIncludeDir, rSONSocketIncludeDir],
	link_with: [rSON],
	dependencies: [rSONSocketLibs, substrate],
	gnu_symbol_visibility: 'inlineshidden',
	version: meson.project_version(),
	install: true
)

if not meson.is_subproject()
	pkgconfig.generate(
		rSONSocket,
		name: 'rSON Socket',
		filebase: 'rSON_socket',
		libraries: [rSON],
		version: meson.project_version(),
		description: 'Sockets handling for rSON, a simple but effective JSON parser for C++'
	)
else
	rSONSocket_dep = declare_dependency(
		include_directories: rSONSocketIncludeDir,
		link_with: rSONSocket,
		version: meson.project_version()
	)
endif
