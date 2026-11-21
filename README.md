# Курсовая работа: Разработка серверной программы
Программа представляет собой TCP-сервер на языке C++, который принимает подключения от клиентов, проводит аутентификацию по логину и паролю (с использованием хэш-функции SHA-1 и соли), а затем принимает бинарные данные (векторы) и возвращает сумму их элементов.

Сборка и запуск

make

В первом окне:

chmod +x server_static

./server_static -T double -H SHA1 -S server

Во втором окне:

./client_double -H SHA1 -S server

Вывод справки:
./vcalc_server -h, ./vcalc_server --help, ./vcalc_server
./vcalc_server
Тестирование рабочих параметров:
./vcalc_server --run -p 4444

Своя база данных:
Создать файл базы: echo "user:123" > my_base.conf
Запустить:./vcalc_server --run -d my_base.conf

Свой лог-файл:
./vcalc_server --run -l my_log.txt
комбинация параметров: 
./vcalc_server --run -p 33333 -d clients.conf

Функциональное тестирование

Тест 1:Вывод справки при  запуске без параметров
./vcalc_server
Тест 2: Вывод справки при параметре -h
./vcalc_server -h
Тест 3: Запуск сервера с параметрами по умолчанию
rm -f clients.conf server.log
./vcalc_server --run &
Тест 4. Обработка неизвестного параметра
./vcalc_server --unknown_param
Тест 5. Запуск с пользовательской базой данных
./vcalc_server --run
Тест 6. Запуск с пользовательским лог-файлом
pkill vcalc_server
cd ~/server_project
echo "user:P@ssW0rd" > clients.conf

# Запуск с указанием своего лог-файла my_log.txt
./vcalc_server --run -d clients.conf -l my_log.txt &
SERVER_PID=$!
echo "Сервер запущен, PID: $SERVER_PID"
sleep 2
ls -la my_log.txt
cat my_log.txt
Тест 7. Комбинация параметров
# Создание базы
echo "user:testpass" > clients.conf

# Запуск с комбинацией параметров
./vcalc_server --run -p 33333 -d clients.conf -l server.log &
SERVER_PID=$!
sleep 2

# Проверка
ps -p $SERVER_PID
netstat -tlnp | grep 33333
cat server.log

# Остановка
kill $SERVER_PID
Тест 8. Тест устойчивости (10 подключений)
# Запуск сервера
./vcalc_server --run -p 33333 -d test_clients.conf -l stability_test.log &
SERVER_PID=$!
sleep 2

# 10 последовательных подключений через netcat
for i in {1..10}; do
    echo "test" | nc -q 1 127.0.0.1 33333 > /dev/null 2>&1
    echo "Подключение #$i завершено"
done

# Проверка, что сервер все еще работает
ps -p $SERVER_PID

# Просмотр лога
cat stability_test.log

# Остановка
kill $SERVER_PID


