#ifndef INITIALIZATION_H
#define INITIALIZATION_H

#include <math.h>

namespace constants {
	const double KSI = 1.0; // ����������� ������
	const double KBT = 1.0; // ������� ��������� �������
	const double SIGMA = 0.8; // ������� ������
	const double EPSILON = 1.0; // ������������ ���� ������������ �������
	const double STEP = 0.0001; // ��� �������������
	const double LAMBDA = sqrt(2.0 * KBT * (STEP / KSI)); // ������� ��������� ��������
	const double LBOND = 1.0; // ����������� ����� �����
	const double KBOND = 1000; // ��������� �����
}

#endif
