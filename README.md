# Транспортный справочник TransportCatalogue
Визуализация остановок и маршрутов через SVG-файл

## Функциональность
* Поиск кратчайшего пути между остановками
* Построение схематической карты
* Получение информации по заданному маршруту/остановке
* Сериализация базы справочника через Google Protobuf

## Принцип работы
Принимает на вход данные в формате JSON

### 1. Входные данные
Структура входного JSON-файла:
```
{
  "base_requests": [ ... ],
  "render_settings": { ... },
  "routing_settings": { ... },
  "serialization_settings": { ... },
  "stat_requests": [ ... ]
}
```
<details>
<summary>Ключи</summary>

* `base_requests` — массив, описывающий маршруты и остановки
* `render_settings` — словарь, задающий параметры изображения
* `routing_settings` — словарь, задающий параметры движения автобусов
* `serialization_settings` — словарь, задающий сериализованную базу данных
* `stat_requests` — массив запросов к транспортному справочнику
</details>

### 2. Создание Транспортного Справочника
#### base_requests — внесение данных об остановках
```
{
  "type": "Stop",
  "name": "Моя остановка",
  "latitude": 67.001205,
  "longitude": 64.407514,
  "road_distances": {
    "Дом бабушки": 3000,
    "Дом друга": 4300
  }
}
```
<details>
<summary>Ключи</summary>

* `type` — "Stop" (остановка)
* `name` — "Моя остановка" (название остановки)
* `latitude` — широта
* `longitude` — долгота
* `road_distances` — словарь, задающий расстояние до соседних остановок. Ключ — название остановки, значение — целое число в метрах
</details>

#### base_requests — внесение данных о маршрутах
```
{
  "type": "Bus",
  "name": "10",
  "stops": [
    "Моя остановка",
    "Дом бабушки",
    "Дом друга"
  ],
  "is_circle": true
}
```
<details>
<summary>Ключи</summary>

* `type` — "Bus" (маршрут)
* `name` — "10" (название маршрута)
* `stops` — массив остановок
* `is_circle` — признак кольцевого маршрута (значение типа bool)
*У кольцевого маршрута название последней остановки дублирует название первой*
</details>

#### render_settings — внесение данных о выходном SVG-файле
```
{
  "width": 1200.0,
  "height": 1200.0,

  "padding": 50.0,

  "line_width": 14.0,
  "stop_radius": 5.0,

  "bus_label_font_size": 20,
  "bus_label_offset": [7.0, 15.0],

  "stop_label_font_size": 20,
  "stop_label_offset": [7.0, -3.0],

  "underlayer_color": [255, 255, 255, 0.85],
  "underlayer_width": 3.0,

  "color_palette": [
    "green",
    [255, 160, 0],
    "red"
  ]
}
```
<details>
<summary>Ключи</summary>

* `width`, `height` — ключи, задающие ширину и высоту изображения в пикселях (вещественные числа [0, 100000])
* `padding` — отступ краёв карты от границ SVG-документа (вещественное число [0, min(width, height)/2))
* `line_width` — толщина линий, которыми рисуются автобусные маршруты (вещественное число [0, 100000])
* `stop_radius` — радиус окружностей, которыми обозначаются остановки (вещественное число [0, 100000])
* `bus_label_font_size` — размер текста, которым написаны названия автобусных маршрутов (целое число [0, 100000])
* `bus_label_offset` — смещение надписи с названием маршрута относительно координат конечной остановки на карте (вещественные числа [-100000, 100000])
* `stop_label_font_size` — размер текста, которым отображаются названия остановок (целое число [0, 100000])
* `stop_label_offset` — смещение названия остановки относительно её координат на карте (вещественные числа [-100000, 100000])
* `underlayer_color` — цвет подложки под названиями остановок и маршрутов
* `underlayer_width` — толщина подложки под названиями остановок и маршрутов. Задаёт значение атрибута stroke-width элемента <text> (вещественное число [0, 100000])
* `color_palette` — цветовая палитра (непустой массив)

*Цвет можно указать одним из способов:*
- строка
- массив из трёх целых чисел [0, 255] — r, g и b компоненты цвета в формате svg::Rgb
- массив из трёх целыъ чисел [0, 255] и одного вещественного числа [0.0, 1.0] — r, g и b компоненты и opacity в формате svg::Rgba
</details>

#### routing_settings — внесение данных об общих параметрах маршрутов
```
"routing_settings": {
      "bus_wait_time": 4,
      "bus_velocity": 60
}
```

<details>
<summary>Ключи</summary>

* `bus_wait_time` — время ожидания автобуса на остановке [1, 1000] (минуты)
* `bus_velocity` — средняя скорость автобуса на маршруте без учёта времени стоянки, разгона и торможения [1, 1000] (км/ч)
</details>

#### serialization_settings — внесение данных из сериализованной базы данных
```
"serialization_settings": {
     "file": "transport_catalogue.db"
}
```
<details>
<summary>Ключи</summary>

* `file` — файл для считывания сериализованной базы данных
</details>

### 3. Запросы к базе транспортного справочника — stat_requests
#### stat_requests — получение информации о маршруте
```
{
  "id": 1,
  "type": "Bus",
  "name": "10"
}
```
<details>
<summary>Ключи</summary>

* `id` — уникальный номер запроса типа type
* `type` — "Bus" (маршрут)
* `name` — название маршрута, для которого нужно вывести информацию
</details>

<details>
<summary>Ответ</summary>

```
{
  "curvature": 2.18604,
  "request_id": 1,
  "route_length": 9300,
  "stop_count": 4,
  "unique_stop_count": 3
} 
```

<details>
<summary>Ключи</summary>

* `curvature` — извилистость маршрута (отношение длины дорожного расстояния маршрута к длине географического расстояния)
* `request_id` — id соответствующего запроса
* `route_length` — длине маршрута (метры)
* `stop_count` — общее количество остановок на маршруте
* `unique_stop_count` — количество уникальных остановок на маршруте
</details>

</details>

#### stat_requests — получение информации об остановке
```
{
  "id": 2,
  "type": "Stop",
  "name": "Моя остановка"
}
```
<details>
<summary>Ключи</summary>

* `id` — уникальный номер запроса типа type
* `type` — "Stop" (остановка)
* `name` — название остановки, для которой нужно вывести информацию
</details>

<details>
<summary>Ответ</summary>

```
{
  "buses": [
      "10", "10А"
  ],
  "request_id": 2
} 
```

<details>
<summary>Ключи</summary>

* `buses` — маршруты автобусов, проходящих через заданную остановку
* `request_id` — id соответствующего запроса
</details>

</details>

#### stat_requests — получение SVG-файла
```
{
  "type": "Map",
  "id": 3
}
```
<details>
<summary>Ключи</summary>

* `type` — "Map" (изображение)
* `id` — уникальный номер запроса типа type
</details>

<details>
<summary>Ответ</summary>

```
{
  "map": "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">...\n</svg>",
  "request_id": 3
} 
```
<details>
<summary>Ключи</summary>

* `map` — изображение маршрутов в формате SVG
* `request_id` — id соответствующего запроса
</details>

</details>

#### stat_requests — построение маршрута между двумя остановками
```
{
  "type": "Route",
  "from": "Моя остановка",
  "to": "Дом бабушки",
  "id": 4
}
```
<details>
<summary>Ключи</summary>

* `type` — "Route" (маршрут между двумя остановками)
* `id` — уникальный номер запроса типа type
* `from` — начальная точка маршрута
* `to` — конечная точка маршрута
</details>

<details>
<summary>Ответ</summary>

```
{
  "items": [
    {
      "stop_name": "Моя остановка",
      "time": 6,
      "type": "Wait"
    },
    {
      "bus": "10",
      "span_count": 1,
      "time": 5.235,
      "type": "Bus"
    }
  ],
  "request_id": 4,
  "total_time": 11.235
}
```
<details>
<summary>Ключи</summary>

* `time` — время ожидания автобуса
* `span_count` — количество остановок, которое нужно проехать на данном автобуса
* `total_time` — общее время в пути
</details>

</details>

## Требования
C++17, Protobuf, CMake

## Сборка
1. Установить [Protobuf](https://github.com/protocolbuffers/protobuf)
2. Скомпилировать proto-файл:
```
<path_to_protobuf>\bin\proto --cpp_out . transport_catalogue.proto
```
4. Собрать проект с помощью CMake:
```
cmake . -DCMAKE_PREFIX_PATH=<path_to_protobuf>/package
cmake --build .
```

*В папки include и lib можно добавить зависимости проекта — Additional Include Directories и Additional Dependencies*

## Планы по доработке
- [ ] Реализация возможности добавления и наложения слоёв (например, спутниковый снимок)
- [ ] Создание трека для навигаторов Garmin ([*GPX*](https://ru.wikipedia.org/wiki/GPX))
- [ ] Создание трека из маршрутных точек для дронов
