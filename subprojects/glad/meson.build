project('glad', 'c', version: '21.05.25')

sources = [
    'src/egl.c',
    'src/gl.c',
    'src/volk.c'
]

libglad = static_library(
    'glad',
    sources,
    include_directories: include_directories('include')
)

libglad_dep = declare_dependency(
    include_directories: include_directories('include'),
    link_with: libglad
)

meson.override_dependency(
    'glad',
    libglad_dep
)