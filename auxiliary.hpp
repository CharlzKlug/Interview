#include <SFML/Graphics.hpp>

constexpr int WINDOW_X = 1024 ; // длина окна по оси X
constexpr int WINDOW_Y = 768 ; // высота окна по оси Y
constexpr int MAX_BALLS = 300 ; // максимальное количество шаров
constexpr int MIN_BALLS = 100 ; // минимальное количество шаров
constexpr float RADIUS_CONST = 10 ; // константа для задания радиуса шара
constexpr float SPEED_CONST = 60 ; // константа для задания скорости шара

// Структура описывающая шар
struct Ball {
  sf::Vector2f p ; // положение шара в окне
  sf::Vector2f dir ; // единичный вектор направления шара
  float r = 0 ; // радиус шара
  float speed = 0 ; // скорость движения шара
} ;

// функция отрисовки шара
void draw_ball (sf::RenderWindow& window, const Ball& ball) ;

// предиктор контакта между двух шаров
bool is_interact (const Ball& ball1, const Ball& ball2) ;

// обработка столкновения шаров с границами окна
void process_bounds_collide (Ball* ball) ;

// анализ близости двух шаров и если, два шара находятся в контакте,
// или шар сталкивается с границами, то запускается корректировка
// направления движения шаров и их скоростей
void collide_process (int** ttc, Ball* balls, int ballsCount,
		      float timeQuantum) ;

// сдвигается шар на количество времени flyTime
void move_ball (Ball& ball, float flyTime) ;

// инициализация матрицы удалённости шаров, выступает как предсказатель
// столкновения двух шаров между собой
void init_collision_matrix (int ** matrix, const Ball * balls,
			    int ballsCount) ;

// обработка столкновения двух шаров
void balls_collision (Ball *ball1, Ball *ball2) ;

// выполняется сдвиг шаров и обрабатывается столкновение
void move_balls (Ball *balls, int ballsNum, int **ttc,
		 float timeQuantum, float deltaTime) ;
