project('RethinkDB C++ driver', 'cpp', default_options: ['cpp_std=c++14'])

thread_dep = dependency('threads')

subdir('src')

include_dir = include_directories('src')
python3 = find_program('python3')

src_dir = meson.current_source_dir()
protobuf_output = meson.build_root()

generic_header = custom_target('protocol_defs', 
                                output: ['protocol_defs.h'], 
                                command: [python3, 
                                                src_dir + '/reql/gen.py', 
                                                src_dir + '/reql/ql2.proto',
                                                'subprojects/rethinkdb/src/protocol_defs.h'
                                                #'@OUTPUT0@' 
                                          ]
                                )

rethinkdb_dep = declare_dependency(include_directories: include_dir, sources: project_srcs, dependencies: thread_dep)

library('rethinkdb++', generic_header, include_directories: [include_dir], sources: project_srcs, 
        dependencies: thread_dep)
static_library('rethinkdb++', generic_header, include_directories: [include_dir], sources: project_srcs, 
        dependencies: thread_dep)

