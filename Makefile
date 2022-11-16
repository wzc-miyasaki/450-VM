
OBJ= client.o serverM.o serverC.o global.o backend.o serverCS.o serverEE.o courseinfo.o courseQuery.o
OUT= client serverM serverC serverCS serverEE 
CXX= g++
FLAGS= -Wall -std=c++11

all : $(OUT)

serverM : serverM.o global.o backend.o
	$(CXX) $(FLAGS) -o $@ $^

serverC : serverC.o global.o
	$(CXX) $(FLAGS) -o $@ $^

serverCS : serverCS.o global.o courseinfo.o courseQuery.o
	$(CXX) $(FLAGS) -o $@ $^

serverEE : serverEE.o global.o courseinfo.o courseQuery.o
	$(CXX) $(FLAGS) -o $@ $^

client : client.o global.o
	$(CXX) $(FLAGS) -o $@ $^

%.o:%.cpp
	$(CXX) $(FLAGS) -c -o $@ $^


.PHONY:clean
clean:
	rm $(OUT)
	rm $(OBJ)
