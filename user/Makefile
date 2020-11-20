PWD 		:= $(shell pwd)
CXX			?= g++
CXXFLAGS 	?= -Wall -DBUILD_DIR=$(BUILD_DIR)
INCLUDE 	?= -I$(PWD)/../netlink
SRC 		?= user.cpp
TARGET		?= user_app


all:
	$(CXX) $(CXXFLAGS) $(SRC) $(INCLUDE) -o $(BUILD_DIR)$(TARGET)

clean:
	rm -rf *.o