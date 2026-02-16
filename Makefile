# Компилятор
CXX := g++
CXXFLAGS := -std=c++17 -Wall -pthread -Iinclude

# Папки
SRCDIR_SERVER := server
SRCDIR_CLIENT := client
INCDIR := include
BUILDDIR := build
BINDIR := bin

# Файлы
SERVER_SRC := $(SRCDIR_SERVER)/main.cpp $(SRCDIR_SERVER)/server.cpp
CLIENT_SRC := $(SRCDIR_CLIENT)/main.cpp $(SRCDIR_CLIENT)/client.cpp
SERVER_BIN := $(BINDIR)/server_app
CLIENT_BIN := $(BINDIR)/client_app

# Создание директорий для сборки и бинарников
$(shell mkdir -p $(BUILDDIR) $(BINDIR))

# Все цели по умолчанию
all: $(SERVER_BIN) $(CLIENT_BIN)

# Сборка сервера
$(SERVER_BIN): $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Сборка клиента
$(CLIENT_BIN): $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Очистка
clean:
	rm -rf $(BUILDDIR)/* $(BINDIR)/*

# Файл PHONY для правильной работы команд
.PHONY: all clean
