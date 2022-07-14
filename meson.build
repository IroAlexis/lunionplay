project(
  'lunionplay', 'c',
  license: 'GPL3+',
  version: '0.4.alpha',
  meson_version: '>= 0.53.0',
  default_options: ['warning_level=2']
)


##########
## Depedencies
##########
glib_dep = dependency('glib-2.0', version: '>= 2.64.0')


sources = [
  'src/config.c',
  'src/debug.c',
  'src/dxvk.c',
  'src/game.c',
  'src/main.c',
  'src/session.c',
  'src/utils.c',
  'src/vkd3d.c',
  'src/wine.c'
]


executable(
  'lunionplay',
  sources,
  dependencies: glib_dep,
  install: true
)
