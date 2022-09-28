doctest_dep = dependency('doctest')

test_sources = [
    'conjurers/string_conjurer_test.cpp',
    'conjurers/file_conjurer_test.cpp',
    'main.cpp',
    'node_test.cpp',
]

test_exe = executable(
    'sourcerer_tests',
    test_sources,
    dependencies: [sourcerer_dep, doctest_dep],
)

test('sourcerer_tests', test_exe)
