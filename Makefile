CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
LIBS = -lcrypto++ -lpthread
TEST_LIBS = -lUnitTest++

# Настройки для основной программы
TARGET = vcalc_server
SRCS = main.cpp server.cpp auth.cpp
OBJS = $(SRCS:.cpp=.o)

# Настройки для тестов
TEST_TARGET = tests
TEST_SRCS = test_server.cpp auth.cpp
TEST_OBJS = test_server.o auth.o

# Цель по умолчанию (собирает только программу)
all: $(TARGET)

# Сборка основной программы
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Сборка тестов (запускается командой make test)
test: $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) $(TEST_OBJS) $(LIBS) $(TEST_LIBS)

# Компиляция любого .cpp файла в .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Очистка проекта
clean:
	rm -f $(TARGET) $(OBJS) $(TEST_TARGET) $(TEST_OBJS)
