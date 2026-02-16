#!/bin/bash

# --- Аргументы ---
SERVER_BIN=./bin/server_app
CLIENT_BIN=./bin/client_app

FILE_PATH=${1:-/tmp/file_src}  # первый аргумент: путь к файлу
PORT=${2:-4040}                # второй аргумент: порт
CHUNK=${3:-1024}               # третий аргумент: размер чанка
NUM_CLIENTS=${4:-1}            # четвёртый аргумент: количество клиентов

# --- Создаём тестовый файл если не существует ---
if [ ! -f "$FILE_PATH" ]; then
    echo "Привет! Это тестовый файл для передачи." > "$FILE_PATH"
    echo "Тестовый файл создан: $FILE_PATH"
fi

# --- Запуск сервера ---
$SERVER_BIN -p $PORT &
SERVER_PID=$!
echo "Сервер запущен на порту $PORT с PID $SERVER_PID"
sleep 1   # ждём, чтобы сервер успел подняться

# --- Запуск клиентов ---
for i in $(seq 1 $NUM_CLIENTS); do
    DST_FILE="/tmp/file_dst${i}"
    $CLIENT_BIN -src 127.0.0.1:$FILE_PATH -dst $DST_FILE -p $PORT -chunk $CHUNK &
    echo "Клиент $i запущен, файл будет сохранён как $DST_FILE"
done

# --- Ждём завершения всех клиентов ---
wait

# --- Завершаем сервер ---
kill $SERVER_PID
echo "Сервер остановлен"
