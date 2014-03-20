rem Создаёт указанную структуру каталогов
rem Если какой-либо каталог уже создан, его пересоздания не происходит
call :MakeDir Country
cd Country
call :MakeDir People
call :MakeDir City
call :MakeDir Ministry
cd People
call :MakeDir Man
call :MakeDir Woman
call :MakeDir Undefined
cd ../City
call :MakeDir Small
call :MakeDir Madiate
call :MakeDir Big
cd ../Ministry
call :MakeDir Financical
call :MakeDir Defence
call :MakeDir Education

goto end

:MakeDir
 if exist %1 goto end
 md %1
:end
