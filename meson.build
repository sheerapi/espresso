project(
	'espresso',
	['cpp', 'c'],
	version: '0.0.1',
	default_options: [
		'cpp_std=c++17',
	],
)

sources = [
	'src/core/Application.cpp',
	'src/core/log.cpp',
	'src/platform/Window.cpp',
	'src/platform/EventHandler.cpp',
	'src/core/EventManager.cpp',
	'src/utils/StringUtils.cpp',
	'src/utils/Demangle.cpp',
	'src/platform/Thread.cpp',
	'src/platform/ThreadManager.cpp',
	'src/graphics/GraphicDevice.cpp',
	'src/graphics/RenderThread.cpp',
	'src/graphics/ogl/GLGraphicDevice.cpp',
	'src/platform/AssetManager.cpp',
	'src/utils/PerformanceTimer.cpp',
	'src/core/Scene.cpp',
	'src/core/TickThread.cpp',
	'src/core/Transform.cpp'
]

if get_option('debug')
	add_project_arguments(['-DDEBUG'], language: 'cpp')
endif

dependencies = [
	dependency('sdl2'),
	dependency('glad'),
	dependency('luajit'),
	dependency('physfs'),
	dependency('libzstd')
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

executable('eapkc', 'tools/AssetCompressor.cpp', dependencies: dependency('libzstd'))
executable('eapkd', 'tools/AssetDecompressor.cpp', dependencies: dependency('libzstd'))

executable('main', dependencies: [expresso_dep], sources: 'sandbox/main.cpp')