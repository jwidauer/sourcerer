json_dep = dependency('nlohmann_json')

sources = [
    'node.cpp'
]

# These arguments are only used to build the shared library
# not the executables that use the library.
lib_args = ['-DBUILDING_SOURCERER']

lib = library('sourcerer',
    sources, 
    include_directories: include_dirs,
    dependencies: json_dep,
    install: true,
    cpp_args: lib_args,
    gnu_symbol_visibility: 'hidden'
)
