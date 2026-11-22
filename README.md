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

Запуск
С параметрами по умолчанию (порт 33333, clients.conf, server.log)
./vcalc_server --run

С указанием параметров
./vcalc_server --run -p 4444 -d my_base.conf -l my_log.txt

Функциональное тестирование
Тест 1. Вывод справки без параметров
./vcalc_server

Тест 2. Вывод справки с -h
./vcalc_server -h

Тест 3. Неизвестный параметр
./vcalc_server --unknown_param

Тест 4. Запуск с параметрами по умолчанию
pkill vcalc_server 2>/dev/null
./vcalc_server --run &
SERVER_PID=$!
sleep 2
ps -p $SERVER_PID
ss -tlnp | grep 33333
cat server.log
kill $SERVER_PID

Тест 5. Запуск с пользовательской базой
echo "user:123" > my_base.conf
./vcalc_server --run -d my_base.conf -p 44444 &
SERVER_PID=$!
sleep 2
ps -p $SERVER_PID
ss -tlnp | grep 44444
kill $SERVER_PID

Тест 6. Запуск с пользовательским логом
./vcalc_server --run -l my_log.txt -p 44444 &
SERVER_PID=$!
sleep 2
ls -la my_log.txt
cat my_log.txt
kill $SERVER_PID

Тест 7. Комбинация параметров
echo "user:testpass" > clients.conf
./vcalc_server --run -p 33333 -d clients.conf -l server.log &
SERVER_PID=$!
sleep 2
ps -p $SERVER_PID
ss -tlnp | grep 33333
cat server.log
kill $SERVER_PID

Тест 8. Полный цикл работы 
Терминал 1:
echo "user:P@ssW0rd" > clients.conf
./vcalc_server --run -p 33333 -d clients.conf -l server.log
Терминал 2:
./client_double -H SHA1 -S server

Тест 9. Тест устойчивости (10 подключений)
./vcalc_server --run -p 33333 -d clients.conf -l stability_test.log &
SERVER_PID=$!
sleep 2
for i in {1..10}; do
    echo "--- Подключение #$i ---"
    ./client_double -H SHA1 -S server
done
ps -p $SERVER_PID
cat stability_test.log
kill $SERVER_PID

Тест 10. Ошибка аутентификации
Терминал 1:
echo "user:wrong_password" > wrong_db.conf
./vcalc_server --run -p 33333 -d wrong_db.conf
Терминал 2:
./client_double -H SHA1 -S server
