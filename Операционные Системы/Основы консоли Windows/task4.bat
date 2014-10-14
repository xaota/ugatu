rem Переименовывает файлы по маске %1 в числовую последовательность

set /a cnt=1
for %%i in (%1) do (
  call :RENAME %%i
  SET /a cnt+=1
)

goto :eof

:RENAME
rem Переименовывает файл %1 согласно значению переменной cnt
rename %1 %cnt%

