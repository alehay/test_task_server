# test_task_server

## Постановка задачи

Разработать консольное серверное приложение на языке С++ работающее в среде ОС LINUX на пользовательском уровне. Приложение должно генерировать для клиента 3 последовательности, каждая из которых представляет собой целочисленный неотрицательный 64-х битный счетчик. Для каждой подпоследовательности начальное значение и шаг между двумя соседними значениями задается клиентом (пользователем) произвольно.
Формат задания параметров – простой текст в tcp/ip сокет (для проверки используется telnet-клиент).

### Перечень команд
 
    1 seq1 xxxx yyyy (задать начальное значение = xxxx и шаг = yyyy для первой подпоследовательности);
    2 seq2 xxxx yyyy (задать начальное значение = xxxx и шаг = yyyy для второй подпоследовательности);
    3 seq3 xxxx yyyy (задать начальное значение = xxxx и шаг = yyyy для третьей подпоследовательности);
    4 export seq - выдавать в сокет клиенту сгенерированную последовательность.

### Примечания
 
    1 Если в командах 1, 2, 3 любой из параметров (начальное значение и/или шаг) будет указан как = 0, то программа не должна генерировать данную последовательность;
    
    2 При переполнении счетчика подпоследовательность должна начинаться сначала (с начального заданного значения xxxx);
    
    3 Для удобства визуализации результата использовать текстовое представление сгенерированных чисел (для отображения в telnet-клиенте);
    
    4 Программа не должна аварийно завершать работу в случаях некорректно введенных параметров, аварийного завершения работы клиента и т. д;
    
    5 Для разработки использовать только стандартные возможности библиотеки языка C++ (11,14,17), проект должен просто собираться (make/cmake/...);
    
    6 Приложение должно быть многопоточным с использованием общего ресурса (единого хранилища настроек клиентов);
    
    7 С каждым пользователем ведётся обособленный диалог (пользователи не знают о действиях друг друга);
    
    8 По возможности использовать шаблоны проектирования;
    
    9 Комментарии в коде приветствуются;
    
    10 Прежде чем сдавать тестовое, проверьте и протестируйте у себя все пункты.

Примеры входных и выходных данных
Необходимо сгенерировать 3 следующие последовательности:
Начальное значение 1, шаг 2: 1, 3, 5 …
Начальное значение 2, шаг 3: 2, 5, 8 …
Начальное значение 3, шаг 4: 3, 7, 11 …
Команды:

    seq1 1 2
    seq2 2 3
    seq3 3 4
    export seq

последняя команда передаёт в сокет последовательность:
    
    1	2	3
    3	5	7
    5	8	11
    ...


## Сборка сервера


    mkdir build && cd ./build 

    cmake -DCMAKE_BUILD_TYPE=Debug ../    

    или

    cmake -DCMAKE_BUILD_TYPE=Release
    
    cmake --build .

## Запуск программы


    ./server 8000