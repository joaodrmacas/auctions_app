CXX = g++
CXXFLAGS = -std=c++11 -Wall -fsanitize=address -fno-omit-frame-pointer -g

SRC_USER = user/commands.cpp user/user.cpp validations.cpp
OBJ_USR = $(SRC_USER:.cpp=.o)

SRC_SV = server/server.cpp validations.cpp server/requests.cpp
OBJ_SV = $(SRC_SV:.cpp=.o)

TARGET_USER = user/user
TARGET_SV = server/AS

.PHONY: all

all: user server

server: $(TARGET_SV)

$(TARGET_SV): $(OBJ_SV)
	$(CXX) -o $(TARGET_SV) $(OBJ_SV)

user: $(TARGET_USER)

$(TARGET_USER): $(OBJ_USR)
	$(CXX) -o $(TARGET_USER) $(OBJ_USR)

%.o: %.cpp
	$(CXX) -c $< -o $@

clean:
	rm -f $(OBJ_USR) $(OBJ_SV) $(TARGET_USER) $(TARGET_SV)
