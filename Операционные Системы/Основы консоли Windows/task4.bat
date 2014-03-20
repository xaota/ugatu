rem Переименовывает файлы по маске %1 в числовую последовательность
set /a cnt = 1
for %%i in (%1) do (
  renamer %%i
  SET /a cnt+=1
)
