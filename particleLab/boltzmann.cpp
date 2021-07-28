/*#include "boltzmann.h"


Boltzmann::Boltzmann(const int n_x, const int n_y, const int n_z){
	int v[9][2];
	double t[15];
	double f_in[n_x][n_y][9];
	double f_out[n_x][n_y][9];
	double u[2];
}

void Boltzmann::init() {

}

void Boltzmann::collision() {
	double density;
	double feq;
	for (int i = 0; i < n_x; i++) {
		for (int j = 0; j < n_y; j++) {
			density = get_density(i, j);
			get_velocity(i, j, density, this->u);
			double u2 = this->u[0] * this->u[0] + this->u[1] * this->u[1];
			double p = 0;
			for (int k = 0; k < 9; k++) {
				for (int d = 0; d < 2; d++) {
					p += this->v[k][d] * u[d];
				}
				feq = this->t[k] * density * (1.0 + 3 * p + (9 / 2) * p * p / 2 - (3 / 2) * u2 / 2);
				f_out[i][j][k] = f_in[i][j][k] + 1 / (2 * 3.14) * (feq - f_in[i][j][k]);
			}
		}
	}
}

void Boltzmann::propagation() {
	for(int i=0; i<n_x; i++)
	{
		for(int j=0; j<n_y; j++)
		{
			for(int k=0; k<9; k++)
			{
				f_in[i][j][oppositeOf[k]]= f_out[i][j][k];					
			}
		}
	}
}

double Boltzmann::get_density(int i, int j) {
	double result = 0.0;
	for (int k = 0; k < 15; k++) {
		result += this->f_in[i][j][k];
	}
	return result;
}

void Boltzmann::get_velocity(int i, int j, double p, double *u) {
	for (int d = 0; d < 3; d++) {
		for (int k = 0; k < 15; k++) {
			u[d] += f_in[i][j][k] * this->v[k][d];
		}
		u[d] /= p;
	}
}*/