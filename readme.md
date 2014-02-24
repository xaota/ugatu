# Лабораторные работы УГАТУ ОНФ
Подборка лабораторных работ на факультете

### Информация
* Лабораторные работы должны лежать в папках с названием предмета;
* В `README` файлах полезно описывать работу, чтобы другие люди могли в ней разобраться;
* `Pull Request` принимаются, так что при желании добавить свою работу - не стесняйтесь, добавляйте.

## Как скачать себе всё

    $ git clone git://github.com/XaoZlo/ugatuLabs.git

* Если у Вас клиент GitHub for Windows или Mac, то можно воспользоваться кнопкой `Clone in Windows` или `Clone in Mac`;
* Также можно воспользоваться кнопкой `Download ZIP`.

Время от времени можно обновлять содержимое, используя клиент GitHub с помощью команд `git fetch` и `git pull`.

### Дополнительно
У отдельных предметов может существовать своя независимая ветка, и, в некоторых случаях, удобно скачивать и использовать именно её, а не весь репозиторий, так как она содержит в себе только файлы к конкретному предмету.

    $ git clone git://github.com/XaoZlo/ugatuLabs.git -b %название_ветки%

* Перед нажатием кнопок `Clone in your OS` или `Download ZIP` на сайте можно выбрать ветку, которая будет скачана.

---
## Для разработчиков
> Как создать ветку для конкретного предмета?

Создание независимой ветки в Git осуществляется командами

    $ git checkout --orphan %название_ветки%
    $ git rm -rf .

Далее, всё зависит от того, существовала ли папка конкретного предмета в основной ветке `master`. Если данного предмета не было, то достаточно просто добавить папку предмета с помощью команды `mkdir` и можно начинать работать с ней. Если же в основной ветке находится папка с нужным предметом, то необходимо добавить её и её содержимое в индекс текущей ветки с помощью команды

    $ git checkout master %название_папки%

После того, как папка создана или добавлена, необходимо создать и отправить на сервер инициализирующий ветку коммит

    $ git add .
    $ git commit -a -m "Initial commit for %название ветки%"
    $ git push --set-upstream origin %название_ветки%

Теперь можно пользоваться веткой, как обычно.
> Как отправить `pull_request`?

В подробностях о том, как отправить `pull request`, можно почитать на хабрахабре, вот тут: http://habrahabr.ru/post/125999/