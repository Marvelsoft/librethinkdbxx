project('RethinkDB C++ driver', 'cpp', default_options: ['cpp_std=c++14'])
comp_flags = ['-Wall','-Wno-unused-command-line-argument']
thread_dep = dependency('threads')

subdir('src')

include_dir = include_directories('src')
rethinkdb_dep1 = declare_dependency(include_directories: include_dir, sources: project_srcs, dependencies: thread_dep)

rethinkdb_lib = library('rethinkdb++', include_directories: [include_dir], sources: project_srcs, 
        dependencies: thread_dep)
rethinkdb_dep = declare_dependency(dependencies: rethinkdb_dep1, link_with: rethinkdb_lib)

static_library('rethinkdb++', include_directories: [include_dir], sources: project_srcs, 
        dependencies: thread_dep)

