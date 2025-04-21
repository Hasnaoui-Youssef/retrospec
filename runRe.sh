g++ -c `mysql_config --cflags` src/main.cpp
g++ -o main main.o  src/mysql_analyzer.cpp src/role_graph.cpp `mysql_config --libs`

