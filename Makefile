# Компилятор и флаги
CXX      := g++
CXXFLAGS := -std=c++20 -O2 -Wall -Wextra -Iinclude

# Имя итогового исполняемого файла
TARGET   := build/translator

# Поиск всех .cpp файлов в папке src
SRCS     := $(wildcard src/*.cpp)

# Генерация имен объектных файлов (src/main.cpp -> build/main.o)
OBJS     := $(patsubst src/%.cpp, build/%.o, $(SRCS))

# --- Основные цели ---

# Цель по умолчанию (если просто написать `make`)
all: $(TARGET)

# Линковка объектных файлов в итоговый бинарник
$(TARGET): $(OBJS)
	@mkdir -p build
	$(CXX) $(OBJS) -o $@
	@echo "✅ Сборка успешно завершена: $@"

# Компиляция каждого .cpp в .o
build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Дополнительные цели ---

# Очистка сгенерированных файлов
clean:
	rm -rf build/*.o $(TARGET)

# Полная пересборка
rebuild: clean all

# Тестовый запуск (пример)
run: $(TARGET)
	./$(TARGET) test/test1.pc -o out.c
	gcc out.c -o out
	./out

# Защита от конфликтов с файлами в файловой системе
.PHONY: all clean rebuild run