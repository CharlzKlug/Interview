#include "auxiliary.hpp"
#include <cmath>

/********************************************************
 * Процедура draw_ball отрисовывает шар                 *
 *                                                      *
 *     window -- окно, в котором выполняется отрисовка, *
 *     ball -- отрисовываемый шар.                      *
 ********************************************************/
void draw_ball (sf::RenderWindow& window, const Ball& ball) {
  sf::CircleShape gball ;
  gball.setRadius (ball.r) ;
  gball.setPosition (ball.p.x, ball.p.y) ;
  gball.setFillColor (sf::Color::Green) ;
  window.draw (gball) ;
}

/**********************************************************
 * Функция is_interact выполняет анализ того, что шары    *
 * находятся во взаимодействии и, если шары контактируют, *
 * то возвращается true. Иначе -- false.                  *
 * Для анализа используется определение расстояния между  *
 * центров шаров и, если расстояние между центров шаров   *
 * меньше, либо равно сумме радиусов шаров, то считается, *
 * что имеет место столкновение.                          *
 *                                                        *
 *     ball1 -- первый шар,                               *
 *     ball2 -- второй шар.                               *
 **********************************************************/
bool is_interact (const Ball& ball1, const Ball& ball2) {
  float x = (ball2.p.x + ball2.r) -
    (ball1.p.x + ball1.r) ;
  float y = (ball2.p.y + ball2.r) -
    (ball1.p.y + ball1.r) ;
  float len = sqrt (x * x + y * y) ;
  return (len <= ball1.r + ball2.r) ;
}

/***********************************************************
 * Процедура process_bounds_collide выполняет обнаружение  *
 * столкновения шара с границами. После обнаружения меняет *
 * соответствующие значения вектора.                       *
 *                                                         *
 *     ball -- шар, для которого выполняется анализ        *
 *     столкновения.                                       *
 ***********************************************************/
void process_bounds_collide (Ball* ball) {

  // столкновение с верхней границей
  if (ball->p.y <= 0) {
    ball->dir.y = fabs (ball->dir.y) ;
  }

  // столкновение с левой границей
  if (ball->p.x <= 0) {
    ball->dir.x = fabs (ball->dir.x) ;
  }

  // столкновение с нижней границей
  if (ball->p.y >= WINDOW_Y - (2 * ball->r)) {
    ball->dir.y = - fabs (ball->dir.y) ;
  }

  // столкновение с правой границей
  if (ball->p.x >= WINDOW_X - (2 * ball->r)) {
    ball->dir.x = - fabs (ball->dir.x) ;
  }
}

/************************************************************************
 * Процедура collide_process определяет, находятся ли шары              *
 * на критическом расстоянии и, если расстояние критическое, то         *
 * выполняется выполняется корректировка расстояния и при подтверждении *
 * столкновения запускается процедура balls_collision.                  *
 *                                                                      *
 *     ttc -- матрица с грубыми прикидками расстояний между шарами;     *
 *     balls -- матрица с шарами;                                       *
 *     ballsCount -- количество шаров;                                  *
 *     timeQuantum -- временной квант.                                  *
 ************************************************************************/
void collide_process (int** ttc, Ball* balls, int ballsCount,
		      float timeQuantum) {
  for (int i = 0 ; i < (ballsCount - 1) ; i++) {
    for (int j = i+1 ; j < ballsCount ; j++) {

      // уменьшаем расстояние между i-ым и j-ым шарами на один
      // максимальный радиус
      ttc[i][j] = ttc[i][j] - 1 ;

      // если расстояние между i-ым и j-ым шарами меньше 3-х
      // радиусов, то выполняется корректировка расстояния
      if (ttc[i][j] < 3) {
	float length = std::max((fabs ((balls[i].p.x + balls[i].r) -
				       (balls[j].p.x + balls[j].r))),
				(fabs ((balls[i].p.y + balls[i].r) -
				       (balls[j].p.y + balls[j].r)))) -
	  (balls[i].r + balls[j].r);
	ttc[i][j] = (length / (2 * RADIUS_CONST)) ;

	// если, после корректировки, окажется, что расстояние между
	// шарами меньше или равно одному максимальному радиусу,
	if (ttc[i][j] <= 1) {

	  // то выполняется точный анализ расстояния между центрами
	  // i-го и j-го шаров и, если шары соприкасаются,
	  if (is_interact (balls[i], balls[j])) {
	    
	    // то выполняется обработка коллизии
	    balls_collision (&balls[i], &balls[j]) ;

	    // и сбрасываем предполагаемое расстояние на 3
	    // максимальных радиусов для того, чтобы шары
	    // вышли друг-из-друга.
	    ttc[i][j] = 3 ;
	  }
	}
      }
    }

    // под конец выполняем анализ столкновения с границами
    process_bounds_collide (balls+i) ;
  }
  process_bounds_collide (balls + ballsCount - 1);
}

/**************************************************************
 * Процедура move_ball выполняет сдвиг шара на период времени *
 * flyTime.                                                   *
 *                                                            *
 *     ball -- обрабатываемый шар;                            *
 *     flyTime -- период времени на полёт шара.               *
 **************************************************************/
void move_ball (Ball& ball, float flyTime) {
  float dx = (ball.dir.x * ball.speed * flyTime) ;
  float dy = (ball.dir.y * ball.speed * flyTime) ;
  ball.p.x += dx ;
  ball.p.y += dy ;
}

/*******************************************************************
 * Процедура init_collision_matrix выполняет первичный анализ      *
 * расстояний между шарами и заполняет матрицу matrix расстояниями *
 * между отдельно взятыми двумя шарами. Расстояние между шарами    *
 * измеряется в максимальных радиусах.                             *
 *                                                                 *
 *     matrix -- матрица содержащая расстояния между двумя         *
 *     отдельными шарами;                                          *
 *     balls -- массив с шарами;                                   *
 *     ballsCount -- количество шаров.                             *
 *******************************************************************/
void init_collision_matrix (int ** matrix, const Ball * balls,
			    int ballsCount) {
  for (int i = 0 ; i < (ballsCount - 1) ; i++) {
    for (int j = (i + 1) ; j < ballsCount ; j++) {
      float length = std::max((fabs ((balls[i].p.x + balls[i].r) -
				     (balls[j].p.x + balls[j].r))),
			      (fabs ((balls[i].p.y + balls[i].r) -
				     (balls[j].p.y + balls[j].r)))) -
	(balls[i].r + balls[j].r);
      matrix [i][j] = (length / (2 * RADIUS_CONST)) ;
    }
  }
}

/**********************************************************************
 * Процедура balls_collision выполняет моделирование столкновения     *
 * шаров и меняет направление движения шаров и скорости в зависимости *
 * от массы, скорости и направления шаров.                            *
 *                                                                    *
 *     ball1 -- первый шар;                                           *
 *     ball2 -- второй шар.                                           *
 **********************************************************************/
void balls_collision (Ball *ball1, Ball *ball2) {
  sf::Vector2f perpV ; // вектор от центра первого шара
  // к центру второго
  
  perpV.x = (ball2->p.x + ball2->r) - (ball1->p.x + ball1->r) ;
  perpV.y = (ball2->p.y + ball2->r) - (ball1->p.y + ball1->r) ;

  // находим длину вектора perpV
  float len = sqrt(perpV.x * perpV.x + perpV.y * perpV.y) ;

  // делаем длину вектора perpV длиной в единицу
  perpV.x = perpV.x / len ;
  perpV.y = perpV.y / len ;

  // находим проекции векторов направления движения первого и второго
  // шаров на вектор perpV
  float p1 = ball1->speed * (perpV.x * ball1->dir.x + perpV.y * ball1->dir.y) ;
  float p2 = ball2->speed * (perpV.x * ball2->dir.x + perpV.y * ball2->dir.y) ;

  // если проекция p1 окажется меньше нуля и проекция p2 больше нуля,
  // то завершаем работу.
  if (p1 < 0 && p2>0) { return ; }

  // сумма масс
  float masses = ball1->r + ball2->r ;

  // находим скорости движения для первого (u1) и второго (u2) шаров
  // после столкновения
  float u1 = (2 * ball2->r * p2 / masses) +
    ((ball1->r - ball2->r) * p1 / masses) ;
  float u2 = (2 * ball1->r * p1 / masses) +
    ((ball2->r - ball1->r) * p2 / masses) ;

  // вектор вдоль прямой, перпендикулярной прямой, соединяющей центры шаров
  sf::Vector2f tangV ; 
  tangV.x = perpV.y ;
  tangV.y = - perpV.x ;

  // находим проекции векторов движений на tangV
  float t1 = ball1->speed * (tangV.x * ball1->dir.x + tangV.y * ball1->dir.y) ;
  float t2 = ball2->speed * (tangV.x * ball2->dir.x + tangV.y * ball2->dir.y) ;

  // находим результирующие векторы движения для первого и второго шарами
  sf::Vector2f result1 = perpV * u1 + tangV * t1;
  sf::Vector2f result2 = perpV * u2 + tangV * t2;

  // определяем абсолютные скорости
  float speed1 = sqrt (result1.x * result1.x + result1.y * result1.y) ;
  float speed2 = sqrt (result2.x * result2.x + result2.y * result2.y) ;

  // находим векторы направления с длиной в единицу
  result1 /= speed1 ;
  result2 /= speed2 ;

  // сохраняем полученные данные в шары
  ball1->dir.x = result1.x ;
  ball1->dir.y = result1.y ;
  ball1->speed = speed1 ;

  ball2->dir.x = result2.x ;
  ball2->dir.y = result2.y ;
  ball2->speed = speed2 ;
  return ;
}

/************************************************************
 * Процедура move_balls выполняет перемещение массива шаров *
 *                                                          *
 *     balls -- ссылка на массив с шарами;                  *
 *     ballsNum -- число шаров;                             *
 *     ttc -- массив с предварительными расстояниями между  *
 *     шарами;                                              *
 *     timeQuantum -- временной квант;                      *
 *     deltaTime -- отрезок времени для моделирования       *
 ************************************************************/
void move_balls (Ball *balls, int ballsNum, int **ttc,
		 float timeQuantum, float deltaTime) {
  if (deltaTime < timeQuantum) {
    for (int i = 0 ; i < ballsNum ; i++) {
      move_ball (balls[i], deltaTime) ;
    }
  } else {
    for (float iter = timeQuantum ; iter <= deltaTime ; iter += timeQuantum) {
      for (int i = 0 ; i < ballsNum ; i++) {
	move_ball (balls[i], timeQuantum) ;
      }
    }
  }
  collide_process (ttc, balls, ballsNum, timeQuantum) ;
}
