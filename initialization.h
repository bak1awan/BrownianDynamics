#ifndef INITIALIZATION_H
#define INITIALIZATION_H

#include <math.h>

namespace constants {
	const double KSI = 1.0; // коэффициент трения
	const double KBT = 1.0; // единица измерения энергии
	const double SIGMA = 0.8; // диаметр шарика
	const double EPSILON = 1.0; // элементарная сила отталкивания шариков
	const double STEP = 0.0001; // шаг моделирования
	const double LAMBDA = sqrt(2.0 * KBT * (STEP / KSI)); // разброс случайной величины
	const double LBOND = 1.0; // равновесная длина связи
	const double KBOND = 1000; // жесткость связи
}

#endif
