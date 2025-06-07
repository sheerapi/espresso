project(
	'espresso',
	['cpp', 'c'],
	version: '0.0.1',
	default_options: [
		'cpp_std=c++11',
	],
)

sources = []

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

executable('main', dependencies: [expresso_dep])