# SPDX Licence-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2024 Laurent Fazio <laurent.fazio@gmail.com>

project('libcobs',
        'c',
        default_options: [
          'c_std=gnu11',
          'default_library=static',
        ])

libcobs_src = [
  'src/recv.c',
  'src/send.c'
]

inc = include_directories('include')

install_subdir('include/cobs', install_dir: 'include')

libcobs = library('cobs',
                  libcobs_src,
                  include_directories: inc,
                  version: '1.0.0',
                  soversion: '1',
                  install: true)

libcobs_dep = declare_dependency(include_directories: inc,
                                 link_with: libcobs)

# COBS tests
e = executable('test-cobs',
               'tests/test-cobs.c',
               include_directories: inc,
               link_with : libcobs)
test('COBS', e)
