project('lunion-play', 'c',
  license: 'GPL3+',
  version: '0.4',
  meson_version: '>= 0.60.0',
  default_options: ['warning_level=2']
)


#enable_tests = get_option('enable_tests')


glib_dep = dependency('glib-2.0', version: '>= 2.70.0')


sources = [
  'src/main.c',
  'src/env.c',
  'src/debug.c',
  'src/config.c',
  'src/core.c',
  'src/system.c',
  'src/wine.c'
]
tests = [
  'tests/env.c',
  'src/env.c',
  'src/debug.c'
]

#if enable_tests
#  subdir(tests)
#endif


executable(
  'lunionplay',
  sources,
  dependencies: glib_dep
)

executable(
  'test-lunionplay',
  tests,
  dependencies: glib_dep
)
