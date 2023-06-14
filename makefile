CXX ?= g++

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g
else
    CXXFLAGS += -O2

endif

server: main.cpp  ./log/log.cpp ./CGImysql/sql_connection_pool.cpp ./util/utils.cpp  ./timer/lst_timer.cpp ./http/http_conn.cpp   ./webserver/webserver.cpp ./config/config.cpp 
	$(CXX) -o server  $^ $(CXXFLAGS) -lpthread -lmysqlclient -g

clean:
	rm  -r server