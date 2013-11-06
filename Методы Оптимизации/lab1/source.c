#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

double f(double x, double y){ // Функция из задания
 return pow(x + y, 2) - 2 * pow(x, 2) + 8 * y + 8 + pow(x, 4);
}
double gradX(double x, double y){ // Частная производная f по x
 return 2 * (2 * pow(x, 3) - x + y); // 4x^3 - 2x + 2y
}
double gradY(double x, double y){ // Частная производная f по y
 return 2 * (x + y + 4); // 2x + 2y + 8
}
double dxdx(double x, double y){return 4 * 3 * pow(x, 2) - 2;}
double dydy(double x, double y){return 2.0;}
double dxdy(double x, double y){return 2.0;}
void grad(double *x, double *y){
 double X = *x;
 double Y = *y;
 *x = gradX(X, Y);
 *y = gradY(X, Y);
}
void magic(double *x, double *y){ // D = (матрица вторых производных {{a, b}, {c, d}}); @return D^-1 * grad
 double X = *x;
 double Y = *y;
 double a = dxdx(X, Y);
 double b = dxdy(X, Y);
 double c = b;
 double d = dydy(X, Y);
 double t = 1.0/(a * d - b * c);
 grad(&X, &Y);
 *x = (d * X - b * Y) * t;
 *y = (a * Y - c * X) * t;
}
double phi(double x, double y, double t){
 double dx = gradX(x, y);
 double dy = gradY(x, y);
 return f(x - t * dx, y - t * dy);
}
double delta(double x, double y){
 double minPhi = phi(x, y, -1.0);
 double min = -1.0;
 for(double t = -0.999; t <= 1.0; t += 0.001){
  double tmp = phi(x, y, t);
  if(tmp < minPhi){
   minPhi = tmp;
   min = t;
  }
 }
 return min;
}
bool condition(double X, double Y, double x, double y, double eps){
 return sqrt(pow(x - X, 2) + pow(y - Y, 2)) > eps;
}
int Newton(double x, double y, double eps){
 double X, Y;
 int iter = 0;
 do{
  X = x;
  Y = y;
  magic(&x, &y);
  x = X - x;
  y = Y - y;
  ++iter;
 }while(condition(X, Y, x, y, eps));
 printf("x: %lf, y: %lf, f: %lf\n", x, y, f(x, y));
 return iter;
}
int fastFall(double x, double y, double eps){
 double X, Y, deltaK;
 int iter = 0;
 do{
  X = x;
  Y = y;
  deltaK = delta(X, Y);
  grad(&x, &y);
  //printf("delta: %lf, gradient x:%lf, y:%lf\n", deltaK, x, y);
  x = X - deltaK * x;
  y = Y - deltaK * y;
  ++iter;
  //printf("x: %lf, y: %lf, f: %lf, %d\n", x, y, f(x, y), iter);
 }while(condition(X, Y, x, y, eps));
 printf("x: %lf, y: %lf, f: %lf\n", x, y, f(x, y));
 return iter;
}
int fastNewtonFall(double x, double y, double eps){
 double X,Y,deltaK, eps1 = 1.8;
 int iter = 0;
 do{
  X = x;
  Y = y;
  deltaK = delta(X, Y);
  grad(&x, &y);
  x = X - deltaK * x;
  y = Y - deltaK * y;
  ++iter;
 }while(condition(X,Y,x,y,eps1));
 int r = Newton(x, y, eps);
 //printf("x: %lf, y: %lf, f: %lf\n", x, y, f(x, y));
 printf("Newton iterations: %d, fastFall epsilon = %lf\n", r, eps1);
 return iter;
}
int main(int argc, char *argv[]){
 double  startX = 1.0, startY = 1.0, epsilon = 0.001;
 int r;

 printf("Newton: (%lf, %lf), epsilon = %lf\n", startX, startY, epsilon);
 r = Newton(startX, startY, epsilon);
 printf("iterations: %d\n\n", r);

 printf("fastFall: (%lf, %lf), epsilon = %lf\n", startX, startY, epsilon);
 r = fastFall(startX, startY, epsilon);
 printf("iterations: %d\n\n", r);

 printf("fastNewtonFall: (%lf, %lf), epsilon = %lf\n", startX, startY, epsilon);
 r = fastNewtonFall(startX, startY, epsilon);
 printf("iterations: %d\n\n", r);

 return 0;
}
