project('sourcerer', 'cpp',
    version : '0.1',
    default_options : ['warning_level=3', 'cpp_std=c++17']
)

include_dirs = include_directories('include')

subdir('include/sourcerer')
subdir('src')

# Make this library usable as a Meson subproject.
sourcerer_dep = declare_dependency(
    include_directories : include_dirs,
    link_with : lib
)

subdir('tests')

pkg_mod = import('pkgconfig')
pkg_mod.generate(
    name : 'sourcerer',
    filebase : 'sourcerer',
    description : 'A library for sourcing configurations.',
    subdirs : 'sourcerer',
    libraries : lib,
    version : '0.1'
)
