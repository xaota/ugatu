rem Копирует все *.xml файлы из директории C:\Windows в C:\Users\newDir, создавая эту директорию
rem Запускать от имени Администратора
cd /
cd Users
md newDir
copy "c:\windows\*.xml" "c:\users\newDir"
cd newDir
