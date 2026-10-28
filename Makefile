CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
LIBS = -lcrypto++ -lpthread

# Программа собирается в корне
TARGET = vcalc_server
SRCS = main.cpp server.cpp auth.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
