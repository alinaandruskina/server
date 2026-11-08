# Курсовая работа: Разработка серверной программы
Программа представляет собой TCP-сервер на языке C++, который принимает подключения от клиентов, проводит аутентификацию по логину и паролю (с использованием хэш-функции SHA-1 и соли), а затем принимает бинарные данные (векторы) и возвращает сумму их элементов.

Сборка и запуск

make

В первом окне:

chmod +x server_static

./server_static -T double -H SHA1 -S server

Во втором окне:

./server_double -H SHA1 -S server

Вывод справки:
./vcalc_server -h, ./vcalc_server --help, ./vcalc_server

Тестирование рабочих параметров:
./vcalc_server --run -p 4444

Своя база данных:
Создать файл базы: echo "user:123" > my_base.conf
Запустить:./vcalc_server --run -d my_base.conf

Свой лог-файл:
./vcalc_server --run -l my_log.txt
комбинация параметров: 
./vcalc_server --run -p 33333 -d clients.conf



