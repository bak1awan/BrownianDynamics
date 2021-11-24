#include <iostream>
#include "initialization.h"
#include <vector>
#include <random>
#include <ctime>
#include <chrono>
#include <string>

const int amount = 50;
const double modellingTime = 0.01;
using namespace std;

class Timer
{
private:
	// Псевдонимы типов используются для удобного доступа к вложенным типам
	using clock_t = std::chrono::high_resolution_clock;
	using second_t = std::chrono::duration<double, std::ratio<1> >;

	std::chrono::time_point<clock_t> m_beg;

public:
	Timer() : m_beg(clock_t::now())
	{
	}

	void reset()
	{
		m_beg = clock_t::now();
	}

	double elapsed() const
	{
		return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
	}
};

// Класс для шариков
class Ball {
public:
	// Шарики содержат свои координаты и силы по X и Y
	double m_x;
	double m_y;
	double Fx;
	double Fy;
	double Rx;
	double Ry;
	Ball() : m_x(0), m_y(0), Fx(0), Fy(0), Rx(0), Ry(0)
	{
	}

	Ball(double x, double y) : m_x(x), m_y(y), Fx(0), Fy(0), Rx(0), Ry(0)
	{
	}
};

// Класс для нашей ячейки
class Box {
public:
	// Ячейка содержит свой размер, его половину и массив шариков
	double m_size;
	vector<Ball> balls;

	// Конструктор по умолчанию
	Box() : m_size(1)
	{
	}
	// Конструктор при передаче размера
	Box(double size) : m_size(size)
	{
		balls.reserve(m_size);
	}

	// Функция для добавления шариков
	void appendBall(const Ball& prevBall) {

		bool addable;
		Ball addingBall;

		do {
			addable = true;
			// Инициализируем координаты случайными числами
			double rx = ((rand() * 2) / (static_cast<double>(RAND_MAX) + 1.0)) - 1.0;
			double ry = ((rand() * 2) / (static_cast<double>(RAND_MAX) + 1.0)) - 1.0;
			double r = sqrt(pow(rx, 2) + pow(ry, 2));
			addingBall.m_x = prevBall.m_x + constants::LBOND * rx / r;
			addingBall.m_y = prevBall.m_y + constants::LBOND * ry / r;

			if (addingBall.m_x < 0) addingBall.m_x = addingBall.m_x + m_size;
			if (addingBall.m_x > m_size) addingBall.m_x = addingBall.m_x - m_size;

			if (addingBall.m_y < 0) addingBall.m_y = addingBall.m_y + m_size;
			if (addingBall.m_y > m_size) addingBall.m_y = addingBall.m_y - m_size;

			// Для каждого добавленного шарика проверяем, не "залазиет" ли на него новый шарик
			for (const Ball& ball : balls) {
				double dx = addingBall.m_x - ball.m_x;
				double dy = addingBall.m_y - ball.m_y;

				if (dx < -m_size / 2.0) dx = dx + m_size;
				if (dx > m_size / 2.0) dx = dx - m_size;

				if (dy < -m_size / 2.0) dy = dy + m_size;
				if (dy > m_size / 2.0) dy = dy - m_size;
				if (sqrt(pow(dx, 2) + pow(dy, 2)) > constants::SIGMA) // изменить проверку с SIGMA: ГОТОВО
					continue;
				else {
					addable = false;
					break;
				}	
			}
		} while (!addable);

		balls.push_back(addingBall); // добавляем шарик
	}
};

// Функция для рассчета сил
double F(double r) {
	if (r > 0.5 * constants::SIGMA && r < constants::SIGMA * pow(2, 1.0 / 6.0))
		return ((-48 * constants::EPSILON) / r) * (pow(constants::SIGMA / r, 12) - 0.5 * pow(constants::SIGMA / r, 6));
	else
		return 0.0;
}

double FBond(double r) {
	return constants::KBOND * (r - constants::LBOND);
}

// Функция для рассчета расстояния между двумя шариками
double calculateDistance(double dx, double dy) {
	return sqrt(pow(dx, 2) + pow(dy, 2));
}

// Вычисляем силы между двумя шариками и меняем их
void calculateForces(Ball& b1, Ball& b2, double size) {
	// Посчитали dx и dy
	double dx = b2.m_x - b1.m_x;
	double dy = b2.m_y - b1.m_y;

	if (dx < -size / 2.0) dx = dx + size;
	if (dx > size / 2.0) dx = dx - size;

	if (dy < -size / 2.0) dy = dy + size;
	if (dy > size / 2.0) dy = dy - size;
	// Посчитали правильно расстояние между шариками
	double r = calculateDistance(dx, dy);

	// Посчитали силу между шариками
	double f = F(r);

	// Изменили значения сил для каждого из шариков
	b1.Fx += f * dx / r;
	b2.Fx -= f * dx / r;
	b1.Fy += f * dy / r;
	b2.Fy -= f * dy / r;
}

void calculateBondForces(Ball& b1, Ball& b2, double size) {
	// Посчитали dx и dy
	double dx = b2.m_x - b1.m_x;
	double dy = b2.m_y - b1.m_y;

	if (dx < -size / 2.0) dx = dx + size;
	if (dx > size / 2.0) dx = dx - size;

	if (dy < -size / 2.0) dy = dy + size;
	if (dy > size / 2.0) dy = dy - size;
	// Посчитали правильно расстояние между шариками
	double r = calculateDistance(dx, dy);

	// Посчитали силу между шариками
	double f = FBond(r);

	// Изменили значения сил для каждого из шариков
	b1.Fx += f * dx / r;
	b2.Fx -= f * dx / r;
	b1.Fy += f * dy / r;
	b2.Fy -= f * dy / r;
}

// Генерируем случайные силы для шарика
void generateRandomForces(Ball& b) {
	b.Rx = 2.0 * rand() / RAND_MAX - 1.0;
	b.Ry = 2.0 * rand() / RAND_MAX - 1.0;
}

// Функция для расчета новых координат
void changeCoordinates(Ball& b, double size) {
	// Изменяем значение координат в зависимости от сил
	b.m_x += b.Fx * constants::STEP / constants::KSI + constants::LAMBDA * b.Rx;
	b.m_y += b.Fy * constants::STEP / constants::KSI + constants::LAMBDA * b.Ry;

	// Проверка на граничные условия по x
	if (b.m_x < 0) b.m_x += size;
	if (b.m_x > size) b.m_x -= size;

	// Проверка на граничные условия по y
	if (b.m_y < 0) b.m_y += size;
	if (b.m_y > size) b.m_y -= size;
}

// Зануляем силы у щарика
void nullifyForces(Ball& b) {
	b.Fx = 0;
	b.Fy = 0;
}

// Выводим в файл координаты каждого шарика в отдельной строке
void printCoordinates(FILE* file, Box& box) {
	for (auto& ball : box.balls) {
		fprintf(file, "%f %f\n", ball.m_x, ball.m_y);
	}
}

int main()
{
	Timer t;
	srand(static_cast<unsigned int>(time(0)));
	rand(); // вызов функции rand() необходимо осуществить один раз, так как там что-то фиксится благодаря этому

	// Сгенерировали нашу ячейку
	Box box(20);

	// Заполнили ее молекулами
	Ball firstBall;
	firstBall.m_x = (rand() / (static_cast<double>(RAND_MAX) + 1.0)) * box.m_size;
	firstBall.m_y = (rand() / (static_cast<double>(RAND_MAX) + 1.0)) * box.m_size;
	box.balls.push_back(firstBall);

	for (int i = 0; i < amount - 1; i++)
		box.appendBall(box.balls[i]);

	// Рассчитали количество итераций исходя из шага моделирования и времени моделирования
	int modelTime = static_cast<int>(modellingTime / constants::STEP) + 1;

	// Указатель на файл
	FILE* hfile;

	string fileN = "";

	// Для имен файлов
	// char fileName[25];

	// Для нумерации в именах файлов
	int counter = 0;

	// Запускаем цикл моделирования
	for (int k = 0; k < modelTime; k++)
	{
		if (k % 10 == 0) {
			fileN = "coordinates_" + to_string(counter++) + ".txt";
			fopen_s(&hfile, fileN.c_str(), "w");
			// snprintf(fileName, sizeof(fileName), "coordinates_%d.txt", counter++);
			// fopen_s(&hfile, fileName, "w");
			printCoordinates(hfile, box);
			fclose(hfile);
		}
		for (auto& ball : box.balls) {
			nullifyForces(ball);
		}

		for (auto& ball : box.balls) {
			generateRandomForces(ball);
		}

		for (int i = 0; i < amount - 1; i++) {
			calculateBondForces(box.balls[i], box.balls[i + 1], box.m_size);
			for (int j = i + 2; j < amount; j++)
				calculateForces(box.balls[i], box.balls[j], box.m_size);
		}

		for (auto& ball : box.balls) {
			changeCoordinates(ball, box.m_size);
		}
	}


	fopen_s(&hfile, "time_elapsed.txt", "w");
	fprintf(hfile, "Time elapsed: %f.", t.elapsed());
	fclose(hfile);

	return 0;
}