# Артемов Никита Владиславович
## Вариант 17

### Условие
Задача о нелюдимых садовниках. Имеется пустой участок зем- ли (двумерный массив) и план сада, разбитого на отдельные квад- раты. От 10 до 30 процентов площади сада заняты прудами или камнями. То есть недоступны для ухаживания. Эти квадраты рас- полагаются на плане произвольным образом. Ухаживание за садом выполняют два садовника, которые не хотят встречаться друг дру- гом (то есть, одновременно появляться в одном и том же квадрате). Первый садовник начинает работу с верхнего левого угла сада и пе- ремещается слева направо, сделав ряд, он спускается вниз и идет в обратном направлении, пропуская обработанные участки. Второй садовник начинает работу с нижнего правого угла сада и переме- щается снизу вверх, сделав ряд, он перемещается влево и также идет в обратную сторону. Если садовник видит, что участок сада уже обработан другим садовником или является необрабатывае- мым, он идет дальше. Если по пути какой-то участок занят другим садовником, то садовник ожидает когда участок освободится, что- бы пройти дальше. Садовники должны работать одновременно со скоростями, определяемыми как параметры задачи. Прохождение через любой квадрат занимает некоторое время, которое задает- ся константой, меньшей чем времена обработки и принимается за единицу времени. Создать приложение, моделирующее ра- боту садовников. Каждого садовника представить отдельным клиентом. Сам сад — сервер.

### Общее
Клиенты садовники отправляют свое положение серверу, сервер проверяет участок и возвращает им или работать или ждать когда другой закончит или идти дальше.

### Запуск
Желательно всех запускать примерно одинаково. Иначе может быть ситуация что кто то уже закончит и сервер выключится
4-5
```sh
gcc client.c -o client
gcc server.c -o server -lrt -lpthread

./server 8000 8001

./client 127.0.0.1 8000 1 2  [первый рабочий подключается на первый указанный порт с временем работы на участке 2]

./client 127.0.0.1 8001 2 4  [второй рабочий подключается на второй указанный порт с временем работы на участке 4]
```
6-8
```sh
gcc observer.c -o observer
gcc client.c -o client
gcc server.c -o server -lrt -lpthread

./server 8000 8001 8002

./observer 127.0.0.1 8002 [наблюдатель, если задание на оценку 8 то сколько угодно наблюдателей]

./client 127.0.0.1 8000 1 2  [первый рабочий подключается на первый указанный порт с временем работы на участке 2]

./client 127.0.0.1 8001 2 4  [второй рабочий подключается на второй указанный порт с временем работы на участке 4]
```