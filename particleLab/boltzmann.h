/*#pragma once

#include <math.h>


class Boltzmann {
public:
	Boltzmann(int a, int b, int c);
	void init();
	void collision();
	void propagation();

	int v[15][3];
	double t[15];
	double f_in[n_x][n_y][n_z][15];
	double f_out[n_x][n_y][n_z][15];
	double u[2];
	int NX, NY, NZ;

private:
	double get_density(int i, int j);
	void get_velocity(int i, int j, double density, double *u);
};
*/