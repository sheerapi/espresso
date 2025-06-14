project('glad', 'c', version: '21.05.25')

sources = [
    'src/egl.c',
    'src/gl.c'
]

libglad = shared_library(
    'glad',
    sources,
    include_directories: include_directories('include'),
    dependencies: [
        dependency('egl', required: false),
        dependency('gl', required: false)
    ]
)

libglad_dep = declare_dependency(
    include_directories: include_directories('include'),
    link_with: libglad,
    dependencies: [
        dependency('egl', required: false),
        dependency('gl', required: false)
    ]
)

meson.override_dependency(
    'glad',
    libglad_dep
)