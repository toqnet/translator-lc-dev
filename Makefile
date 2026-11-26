# ==============================================================================
#  Кроссплатформенный Makefile для транслятора (Linux / Windows)
# ==============================================================================

# --- Определение операционной системы ---
ifeq ($(OS),Windows_NT)
    # Настройки для Windows
    TARGET   := build/translator.exe
    RUN_EXE  := out.exe
    RM       := del /Q
    MKDIR    := mkdir
    # В Windows утилита rm может отсутствовать, используем del
    CLEAN_CMD = if exist build\*.o $(RM) build\*.o & if exist $(TARGET) $(RM) $(TARGET)
else
    # Настройки для Linux / macOS
    TARGET   := build/translator
    RUN_EXE  := out
    RM       := rm -rf
    MKDIR    := mkdir -p
    CLEAN_CMD = $(RM) build/*.o $(TARGET)
endif

# --- Компилятор и флаги ---
CXX      := g++
CXXFLAGS := -std=c++20 -O2 -Wall -Wextra -Iinclude

# --- Поиск файлов ---
SRCS     := $(wildcard src/*.cpp)
OBJS     := $(patsubst src/%.cpp, build/%.o, $(SRCS))

# ==============================================================================
#  Основные цели
# ==============================================================================

# Цель по умолчанию
all: $(TARGET)

# Линковка объектных файлов
$(TARGET): $(OBJS)
	@$(MKDIR) build
	$(CXX) $(OBJS) -o $@
	@echo "========================================"
	@echo "✅ Сборка успешно завершена: $@"
	@echo "========================================"

# Компиляция каждого .cpp в .o
build/%.o: src/%.cpp
	@$(MKDIR) build
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ==============================================================================
#  Дополнительные цели
# ==============================================================================

# Очистка сгенерированных файлов
clear:
	@echo "🧹 Очистка..."
	$(CLEAN_CMD)
	@if [ -f out.c ]; then $(RM) out.c; fi
	@if [ -f out ]; then $(RM) out; fi
	@if [ -f out.exe ]; then $(RM) out.exe; fi

# Полная пересборка
rebuild: clean all

# Тестовый запуск (трансляция + сборка + запуск)
run: $(TARGET)
	@echo "🚀 Запуск трансляции..."
	./$(TARGET) test/test1.pc -o out.c
	@echo "🔨 Компиляция C-кода..."
	gcc out.c -o $(RUN_EXE)
	@echo "▶️  Запуск программы..."
	./$(RUN_EXE)

# Защита от конфликтов с файлами
.PHONY: all clean rebuild run