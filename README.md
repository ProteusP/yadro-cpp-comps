# ClubManager

## Компиляция

### GCC

Для простой сборки под gcc на linux можно использовать Cmake:

```bash
mkdir build
cd build && cmake .. && make
mv ./ClubManager ../ && cd ../
```

### MINGW

Команда для сборки:

```PowerShell
g++ main.cpp utils.cpp ClubManager.cpp -o ClubManager.exe -O2 -Wall -Wextra -Wpedantic -Werror
```

### Docker (debian)

Для дальнейших шагов необходимо установить и запустить docker

#### GCC

Компиляция и перенос исполняемого файла:

```bash
docker build -f ./dockerfile_for_lin/fockerfile -t cpp-lin .
docker create --name temp-lin-cont cpp-lin
docker cp temp-lin-cont:/app/ClubManager ./
docker rm temp-lin-cont
```

#### MINGW

Компиляция и перенос исполняемого файла:

```PowerShell
docker build -f ./dockerfile_for_win/dockerfile -t cpp-win .
docker create --name temp-win-cont cpp-win
docker cp temp-win-cont:/app/ClubManager.exe ./
docker rm temp-win-cont
```

## Запуск

### Linux

```bash
./ClubManager <input_file>
```

Например, запуск с данными из ТЗ:

```bash
./ClubManager input.txt
```
### Windows

```PowerShell
ClubManager.exe <input_file>
```

Например, запуск с данными из ТЗ:

```PowerShell
ClubManager.exe input.txt
```

## Тесты

Файлы с тестовыми данными представлены в директории _test-data_:

- test_input_valid.txt - запуск с данным файлом покажет корректно отрабатывающий пример
- test_input_invalid_config.txt - в данном файле некорректно указан формат времени для конфигурации клуба. Программа выведет строчку, в которой нарушен формат
- test_input_invalid_event.txt - в данном файле добавлены лишние аргументы к строке, описывающей событие. Программа выведет данную строку
