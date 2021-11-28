#include <iostream>
#include "initialization.h"
#include <vector>
#include <random>
#include <ctime>
#include <chrono>
#include <string>
#include <algorithm>
#include <numeric>

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
	// Шарики содержат свои координаты и силы по X, Y и Z 
	vector<double> coordinates;

	vector<double> forces;

	vector<double> randomForces;

	Ball() : coordinates{ 0.0, 0.0, 0.0 }, forces{ 0.0, 0.0, 0.0 }, randomForces{ 0.0, 0.0, 0.0 }
	{
	}
};

void checkBorderConditions(vector<double>& coordinates, double size) {
	for_each(coordinates.begin(), coordinates.end(), [size](double& cord) {
		if (cord < 0.0) cord += size;
		if (cord > size) cord -= size;
	});
}

// Функция для рассчета расстояния между двумя шариками
double calculateDistance(vector<double>& coordinates) {
	return sqrt(accumulate(coordinates.begin(), coordinates.end(), 0.0, [](double acc, double cord) {
		return acc + pow(cord, 2);
	}));
}

vector<double> getDistance(const vector<double>& b1, const vector<double>& b2) {
	vector<double> result(3);
	for (int i = 0; i < result.size(); ++i) {
		result[i] = b1[i] - b2[i];
	}
	return result;
}

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
			vector<double> r_coordinates(addingBall.coordinates.size());

			for_each(r_coordinates.begin(), r_coordinates.end(), [](double& cord) {
				cord = ((rand() * 2.0) / (static_cast<double>(RAND_MAX) + 1.0)) - 1.0;
			});

			auto r = calculateDistance(r_coordinates);

			for (int i = 0; i < addingBall.coordinates.size(); i++) {
				addingBall.coordinates[i] = prevBall.coordinates[i] + constants::LBOND * r_coordinates[i] / r;
			}


			checkBorderConditions(addingBall.coordinates, m_size);


			// Для каждого добавленного шарика проверяем, не "залазиет" ли на него новый шарик
			for (const Ball& ball : balls) {
				vector<double> d_distance = getDistance(addingBall.coordinates, ball.coordinates);

				checkBorderConditions(d_distance, m_size / 2.0);

				if (calculateDistance(d_distance) > constants::SIGMA) // изменить проверку с SIGMA: ГОТОВО
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
		return ((-48.0 * constants::EPSILON) / r) * (pow(constants::SIGMA / r, 12) - 0.5 * pow(constants::SIGMA / r, 6));
	else
		return 0.0;
}

double FBond(double r) {
	return constants::KBOND * (r - constants::LBOND);
}

// Вычисляем силы между двумя шариками и меняем их
void calculateForces(Ball& b1, Ball& b2, double size) {
	// Посчитали dx и dy и dz
	vector<double> d_distance = getDistance(b2.coordinates, b1.coordinates);

	checkBorderConditions(d_distance, size / 2);

	// Посчитали правильно расстояние между шариками
	double r = calculateDistance(d_distance);

	// Посчитали силу между шариками
	double f = F(r);

	// Изменили значения сил для каждого из шариков
	for (int i = 0; i < b1.coordinates.size(); i++) {
		b1.forces[i] += f * d_distance[i] / r;
		b2.forces[i] -= f * d_distance[i] / r;
	}
}

void calculateBondForces(Ball& b1, Ball& b2, double size) {
	// Посчитали dx и dy и fz

	vector<double> d_distance = getDistance(b2.coordinates, b1.coordinates);

	checkBorderConditions(d_distance, size);

	// Посчитали правильно расстояние между шариками
	double r = calculateDistance(d_distance);

	// Посчитали силу между шариками
	double f = FBond(r);

	// Изменили значения сил для каждого из шариков
	for (int i = 0; i < b1.randomForces.size(); i++) {
		b1.forces[i] += f * d_distance[i] / r;
		b2.forces[i] -= f * d_distance[i] / r;
	}
}

// Генерируем случайные силы для шарика
void generateRandomForces(Ball& b) {
	// b.Rx = 2.0 * rand() / RAND_MAX - 1.0;
	// b.Ry = 2.0 * rand() / RAND_MAX - 1.0;
	/*
	for (int i = 0; i < b.randomForces.size(); i++) {
		b.randomForces[i] = 2.0 * rand() / RAND_MAX - 1.0;
	}
	*/
	for_each(b.randomForces.begin(), b.randomForces.end(), [](double& cord) {
		cord = 2.0 * rand() / RAND_MAX - 1.0;
	});
}

// Функция для расчета новых координат
void changeCoordinates(Ball& b, double size) {
	// Изменяем значение координат в зависимости от сил
	for (int i = 0; i < b.coordinates.size(); i++) {
		b.coordinates[i] = b.forces[i] * constants::STEP / constants::KSI + constants::LAMBDA * b.randomForces[i];
	}

	checkBorderConditions(b.coordinates, size);

}

// Зануляем силы у щарика
void nullifyForces(Ball& b) {
	b.forces = { 0, 0, 0 };
}

// Выводим в файл координаты каждого шарика в отдельной строке
void printCoordinates(FILE* file, Box& box) {
	for (auto& ball : box.balls) {
		fprintf(file, "%f %f %f\n", ball.coordinates[0], ball.coordinates[1], ball.coordinates[2]);
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
	for_each(firstBall.coordinates.begin(), firstBall.coordinates.end(), [size = box.m_size](double& cord) {
		cord = (rand() / (static_cast<double>(RAND_MAX) + 1.0)) * size;
	});

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