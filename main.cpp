#include "MiddleAverageFilter.h"
#include "auxiliary.hpp"
#include <cmath>

// возвращает равномерно распределённое случайное число
// в отрезке от 0 до 1
#define RANDOM_0_1 (((float) rand ()) / RAND_MAX)

// класс счётчика среднего значения кадров в секунду за последние 100 кадров
Math::MiddleAverageFilter<float, 100> fpsCounter ;

int main () {
  sf::RenderWindow window (sf::VideoMode (WINDOW_X, WINDOW_Y),
			   "ball collision demo") ;
  srand (time(NULL));

  // в этом массиве хранятся все обрабатываемые шары
  Ball balls[MAX_BALLS] ;

  // здесь генерируется количество шаров на отрезке [MIN_BALLS; MAX_BALLS]
  int ballsNumber = ((RANDOM_0_1) * (MAX_BALLS - MIN_BALLS) + MIN_BALLS) ;

  // Создание текста для экрана
  sf::Font font;
  if (!font.loadFromFile("/usr/share/fonts/corefonts/arial.ttf"))
    return EXIT_FAILURE;
  sf::Text text("Hello SFML", font, 10);

  // инициализация шаров
  // задаются положения шаров, направление движения шаров
  // в виде единичного вектора, радиус шара на отрезке
  // [RADIUS_CONST; 2*RADIUS_CONST], и скорость [SPEED_CONST; 2*SPEED_CONST]
  for (int i = 0 ; i < ballsNumber ;
       i++) {

    // положение шара в пространстве
    balls[i].p.x = (RANDOM_0_1) * WINDOW_X ;
    balls[i].p.y = (RANDOM_0_1) * WINDOW_Y ;

    // угол для задания направления шара
    float angle = RANDOM_0_1 * 2 * 3.1415 ;
    balls[i].dir.x = cos (angle) ; // направление по оси X
    balls[i].dir.y = sin (angle) ; // направление по оси Y

    // задание радиуса шара
    balls[i].r = (RADIUS_CONST) + ((RANDOM_0_1) * (RADIUS_CONST)) ;

    // задание скорости движения шара
    balls[i].speed = (SPEED_CONST) + ((RANDOM_0_1) * (SPEED_CONST)) ;
  }

  // временной квант, соответствует времени прохождения шаром
  // расстояния в четверть минимального радиуса c двойной максимальной
  // скоростью
  float timeQuantum = (RADIUS_CONST) / (8 * SPEED_CONST) ;

  // матрица расстояний между шарами до коллизии
  // здесь расстояния считаются в целых значениях максимального радиуса
  int *ttc[MAX_BALLS] ; 
  for (int i = 0 ; i < ballsNumber ; i++) {
    ttc[i] = new int[ballsNumber] ;
  }

  init_collision_matrix (ttc, balls, ballsNumber) ;

  // задание времени 
  sf::Clock clock ;
  float lastTime = clock.restart().asSeconds() ;
  
  while (window.isOpen ()) {
    sf::Event event ;
    while (window.pollEvent (event)) {
      if (event.type == sf::Event::Closed)
	window.close () ;
    }

    // получаем время для обработки движения шаров
    lastTime = clock.getElapsedTime().asSeconds() ;
    clock.restart().asSeconds() ;

    // высчитываем среднее арифметическое fps за последние ста
    // замеров
    fpsCounter.push (1.0f / lastTime) ;

    // выводим среднее арифметическое fps за последние сто замеров
    text.setString ("fps: " +
		    (std::__cxx11::to_string (fpsCounter.getAverage()))) ;

    // проигрываем движения шаров за отрезок времени lastTime
    move_balls (balls, ballsNumber, ttc, timeQuantum, lastTime) ;

    // очищаем окно
    window.clear () ;

    // рисуем шары
    for (int i = 0 ; i < ballsNumber ; i++) {
      draw_ball (window, balls[i]) ;
    }

    // рисуем сообщение про fps
    window.draw(text);

    // отображаем кадр
    window.display () ;
  }

  // очистка памяти от матрицы расстояний
  for (int i = 0 ; i < ballsNumber ; i++) {
    delete [] ttc[i] ;
  }
  
  return (0) ;
}
