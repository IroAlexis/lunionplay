project('lunion-play', 'c',
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
  'src/main.c',
  'src/session.c',
  'src/ini.c',
  'src/debug.c',
  'src/system.c',
  'src/wine.c',
  'src/dxvk.c',
  'src/vkd3d.c'
]


executable(
  'lunionplay',
  sources,
  dependencies: glib_dep
)
