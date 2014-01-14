reset;
set title "Задание 1.3.2";
set mapping cylindrical;
# set parametric;
set xlabel "X";
set ylabel "Y";
set zlabel "Z";

splot (cos(x)**2)/4 + sin(y)**2 - 1/4;
