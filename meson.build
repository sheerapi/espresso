project(
	'espresso',
	['cpp', 'c'],
	version: '0.0.1',
	default_options: [
		'cpp_std=c++11',
	],
)

sources = [
	'src/core/Application.cpp',
	'src/core/log.cpp',
	'src/platfom/Window.cpp',
	'src/platfom/EventHandler.cpp',
	'src/core/EventManager.cpp',
	'src/utils/StringUtils.cpp',
	'src/utils/Demangle.cpp'
]

if get_option('debug')
	add_project_arguments(['-DDEBUG'], language: 'cpp')
endif

dependencies = [
	dependency('sdl2'),
]

espresso_lib = library(
	'espresso',
	sources: sources,
	dependencies: dependencies,
	include_directories: include_directories('include'),
)

expresso_dep = declare_dependency(
	link_with: espresso_lib,
	dependencies: dependencies,
	include_directories: include_directories('include'),
)

executable('main', dependencies: [expresso_dep], sources: 'sandbox/main.cpp')