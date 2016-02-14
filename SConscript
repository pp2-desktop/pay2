env = Environment()

env.ParseConfig('pkg-config --cflags --libs openssl')

env.Append(CXXFLAGS=['-std=c++11', '-Wall', '-g'])

env.Append(CPPPATH=['/usr/local/include'])


env.Append(LIBS=['pthread', 'boost_system', 'boost_regex', 'boost_thread', 'boost_filesystem', 'boost_program_options', 'boost_coroutine', 'boost_context', 'apr-1', 'aprutil-1', 'log4cxx', 'libmysqlcppconn'])


env.Append(LIBPATH=['/usr/local/lib'])


env.Program('xx', ['ws_examples.cpp', 'log4cxx_md.cpp', 'cd_user_md.cpp', 'cd_user.cpp', 'json11.cpp', 'cd_handler_md.cpp', 'lobby_md.cpp', 'play_md.cpp', 'db_md.cpp', 'handler/user_info_req.cpp', 'handler/lobby_req.cpp', 'handler/play_req.cpp', 'game_info.cpp'])
