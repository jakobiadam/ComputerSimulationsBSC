#include <iostream>
#include <fstream>
#include<cmath>

using namespace std;

const double pi = 4 * atan(1.0);
double L, T, rho, dt, dx, t_sum, x00,x01, y00,y01,A00,A01,n00,n01, c, c_;
string mode, save;

double triang(double x) {
	if (x <= x00) {
		return x / x00 * y00;
	} else {
		return (L - x) / (L - x00) * y00;
	}
}

double triangCombine(double x) {
	double h = 0.0;
	if (x <= x00) {
		h += x / x00 * y00;
	} else {
		h += (L - x) / (L - x00) * y00;
	}
	if (x <= x01) {
		h += x / x01 * y01;
	} else {
		h += (L - x) / (L - x01) * y01;
	}
	return h;
}

double mode1(double x) {
	return A00 * sin((pi*n00)/L*x);
}
	
double mode2(double x) {
	return A00 * sin((pi*n00)/L*x) + A01 * sin((pi*n01)/L*x);
}

double dirac(double x) {
	if (x == x00) {
		return y00;
	} else {
		return 0.0;
	}
}

void kezdeti_feltetelek(double *arr1, double *arr2, int length, double f(double)) {
	for (int i=1; i<length-1; ++i) {
		double x = i * dx;
		arr1[i] = f(x);
		arr2[i] = f(x);
	}
}

void hur_leapfrog(double *arr1, double *arr2, int length) {
	double arr_new[length] = {0.0};

	for (int i=1; i<length-1; ++i) {
		arr_new[i] = 2 * arr2[i] - arr1[i] + pow(c/c_,2) * (arr2[i+1] + arr2[i-1] - 2 * arr2[i]);
		arr1[i-1] = arr2[i-1];
		arr2[i-1] = arr_new[i-1];
	}

	arr1[length-2] = arr2[length-2];
	arr2[length-2] = arr_new[length-2];

	/*for (int i=1; i<length-1; ++i) {
		arr1[i] = arr2[i];
		arr2[i] = arr_new[i];
	}*/

}


int main(int argc, char *argv[])
{	
	if (argc != 17) {
		cout << "Nem megfelelő száma argumentum." << endl;
		cout << "A szükséges paraméterek a következők:" << endl;
		cout << "L\nT\nrho\ndt\ndx\nt_sum\nx0\nx1\ny0\ny1\nA0\nA2\nn0\nn1\nmode\nsave\n"; 
		return -1;
	} else {
		L = atof(argv[1]);
		T = atof(argv[2]);
		rho = atof(argv[3]);
		dt = atof(argv[4]);
		dx = atof(argv[5]);
		t_sum = atof(argv[6]);
		x00 = atof(argv[7]);
		x01 = atof(argv[8]);
		y00 = atof(argv[9]);
		y01 = atof(argv[10]);
		A00 = atof(argv[11]);
		A01 = atof(argv[12]);
		n00 = atof(argv[13]);
		n01 = atof(argv[14]);
		mode = argv[15];
		save = argv[16];
	}

	int N_t = t_sum / dt + 1;
	int N_x = L / dx +1;

	t_sum = (N_t - 1.0) * dt;
	L = (N_x - 1.0) * dx;

	cout << "Szimuláció újraszámolt ideje: " << t_sum << endl;
	cout << "Húr újraszámolt hossza: " << L << endl;
	
	c = pow(T/rho,0.5);
	c_ = pow(dx/dt,0.5);
	
	double *y_i = new double [N_x];
	double *y_ii = new double [N_x];
	
	if (mode.compare("triang") == 0) {
		kezdeti_feltetelek(y_i,y_ii,N_x,triang);
	} else if (mode.compare("triangCombine") == 0) {
		kezdeti_feltetelek(y_i,y_ii,N_x,triangCombine);
	} else if (mode.compare("dirac") == 0) {
		kezdeti_feltetelek(y_i,y_ii,N_x,dirac);
	} else if (mode.compare("mode1") == 0) {
		kezdeti_feltetelek(y_i,y_ii,N_x,mode1);
	} else if (mode.compare("mode2") == 0) {
		kezdeti_feltetelek(y_i,y_ii,N_x,mode2);
	} else {
		cout << "Rossz mode lett megadva!" << endl;
	}
	
	ofstream dataFile(save);

	double t = 0;
	do {
		for (int i=0; i<N_x; ++i) {
			dataFile << y_ii[i] << '\t';
		}
		dataFile << '\n';

		hur_leapfrog(y_i,y_ii,N_x);
		
		t = t + dt;
		cout << "t= " << t << endl;
	} while (t<t_sum+dt/10);

	dataFile.close();
	cout << "Data in hur.data" << endl;

}

