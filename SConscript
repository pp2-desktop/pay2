env = Environment()

env.ParseConfig('pkg-config --cflags --libs openssl')

env.Append(CXXFLAGS=['-std=c++11', '-Wall', '-g'])

env.Append(CPPPATH=['/usr/local/include'])


env.Append(LIBS=['pthread', 'boost_system', 'boost_regex', 'boost_thread', 'boost_filesystem', 'boost_program_options', 'boost_coroutine', 'boost_context', 'apr-1', 'aprutil-1', 'log4cxx'])


env.Append(LIBPATH=['/usr/local/lib'])


env.Program('xx', ['ws_examples.cpp', 'cd_user_md.cpp', 'cd_user.cpp', 'json11.cpp', 'cd_handler_md.cpp', 'log4cxx_md.cpp'])
