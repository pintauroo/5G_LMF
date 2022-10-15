
project(
    'httpServer',
    'cpp',
    meson_version: '>=0.55.0'
)

executable(
    'httpServer',
    sources: ['lmf-server.cpp', 'locator.cpp', 'locator.h'],
    dependencies: [dependency('libpistache'), dependency('armadillo')],
    cpp_args : '-larmadillo'
)
