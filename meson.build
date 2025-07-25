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
	'src/platform/AssetManager.cpp',
	'src/utils/PerformanceTimer.cpp',
	'src/core/Scene.cpp',
	'src/core/TickThread.cpp',
	'src/core/Transform.cpp',
	'src/platform/JobScheduler.cpp',
	'src/components/graphics/Camera.cpp',
	'src/platform/assets/LuaScript.cpp',
	'src/components/core/LuaScriptEngine.cpp',
	'src/components/core/LuaBehavior.cpp',

	'src/graphics/GraphicDevice.cpp',
	'src/graphics/RenderThread.cpp',
	'src/graphics/Renderer.cpp',
	'src/graphics/RenderGraph.cpp',
	'src/graphics/vulkan/VkGraphicDevice.cpp',
	'src/graphics/vulkan/VkGraphicContext.cpp',

	'src/graphics/noop/NopGraphicContext.cpp',
	'src/graphics/noop/NopGraphicDevice.cpp',

	'src/core/jobs/FrameMemoryPool.cpp',
	'src/core/jobs/JobManager.cpp',
	'src/core/jobs/WorkerThread.cpp',
	'src/core/jobs/Job.cpp'
]

editor_src = [
	'src/editor/core/EditorContext.cpp'
]

if get_option('debug')
	add_project_arguments(['-DDEBUG'], language: 'cpp')
endif

if get_option('editor')
	add_project_arguments(['-DEDITOR'], language: 'cpp')
	sources += editor_src
endif

dependencies = [
	dependency('sdl2'),
	dependency('glad'),
	dependency('luajit'),
	dependency('libzstd'),
	dependency('cglm'),
	dependency('vulkan-memory-allocator'),
	dependency('shaderc'),
	dependency('imgui_docking'),
	dependency('concurrentqueue'),
	dependency('magic_enum', default_options: {'test': false}),
	subproject('sol2', default_options: {'use_luajit': true}).get_variable('sol2_dep'),
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
