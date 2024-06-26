CXX = g++
# CXXFLAGS = -std=c++11 -Wall -fsanitize=address -fno-omit-frame-pointer -g

SRC_USER = client/commands.cpp client/user.cpp validations.cpp
OBJ_USR = $(SRC_USER:.cpp=.o)

SRC_SV = server/server.cpp validations.cpp server/requests.cpp
OBJ_SV = $(SRC_SV:.cpp=.o)

TARGET_USER = user
TARGET_SV = AS

.PHONY: all

all: client server

server: $(TARGET_SV)

$(TARGET_SV): $(OBJ_SV)
	$(CXX) -std=c++17 -lstdc++ -o $(TARGET_SV) $(OBJ_SV)

client: $(TARGET_USER)

$(TARGET_USER): $(OBJ_USR)
	$(CXX) -std=c++17 -lstdc++ -o $(TARGET_USER) $(OBJ_USR)

%.o: %.cpp
	$(CXX) -std=c++17 -lstdc++ -c $< -o $@

clean:
	rm -f $(OBJ_USR) $(OBJ_SV) $(TARGET_USER) $(TARGET_SV)

run_user:
	./$(TARGET_USER)

run_server:
	./$(TARGET_SV)
