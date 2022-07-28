#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include "vector.hpp"        // vectors with components of type double
#include "odeint.hpp"        // ODE integration routines, Runge-Kutta ...
using namespace cpl;

const double pi = 4 * atan(1.0);

const double g = 9.8;        // acceleration of gravity
const double m = 1; // az inga tömegét egységnyinek választottam, nincs igazán jelentősége

double L = 1.0;              // length of pendulum
double q = 0.5;              // damping coefficient
double Omega_D = 2.0/3.0;    // frequency of driving force
double F_D = 0.9;            // amplitude of driving force
bool nonlinear;              // linear if false

Vector f(const Vector& x) {  // extended derivative vector
    double t = x[0];
    double theta = x[1];
    double omega = x[2];
    Vector f(3);             // Vector with 3 components
    f[0] = 1;
    f[1] = omega;
    if (nonlinear)
        f[2] = - (g/L) * sin(theta) - q * omega + F_D * sin(Omega_D * t);
    else
        f[2] = - (g/L) * theta - q * omega + F_D * sin(Omega_D * t);
    return f;
}

void EulerStep(Vector& x, double tau,
             Vector derivs(const Vector&))
{
    Vector k1 = tau * derivs(x);
    x += k1;
}

void EulerCromerStep(Vector& x, double tau,
						Vector derivs(const Vector&))
	{
	Vector k1 = tau * derivs(x);
	x[0] += k1[0];
	x[2] += k1[2];
	x[1] += tau * x[2];
	}
	
double Energy(const Vector& x) {
	return m * g * L * (1 - cos(x[1])) + 1 / 2 * m * pow(L * x[2],2);
}

int main(int argc, char *argv[]) {
	double theta, omega, tMax;
	int mode;
	if (argc==10) { // nem kezeltem a rossz inputokat, nem felhasználóbarát a kód.
		nonlinear = argv[1][0] == 'n';
		L = atof(argv[2]);
		q = atof(argv[3]);
		Omega_D = atof(argv[4]);
		F_D = atof(argv[5]);
		theta = atof(argv[6]);
		omega = atof(argv[7]);
		tMax = atof(argv[8]);
		mode = atoi(argv[9]);
		
	} else {
		cout << "9 parancssori argumentum kell, melyek rendre a következők: \n nonlinear \n L \n q \n Omega_D \n F_D \n theta \n omega \n tMax \n mode (1 : Euler ; 2 : EulerCromer ; 3 : RK4 ; 4 : adaptiveRK4)" << endl;
		return -1;
	}
	/*
    cout << " Nonlinear damped driven pendulum\n"
         << " --------------------------------\n"
         << " Enter linear or nonlinear: ";
    string response;
    cin >> response;
    nonlinear = (response[0] == 'n');
    cout<< " Length of pendulum L: ";
    cin >> L;
    cout<< " Enter damping coefficient q: ";
    cin >> q;
    cout << " Enter driving frequencey Omega_D: ";
    cin >> Omega_D;
    cout << " Enter driving amplitude F_D: ";
    cin >> F_D;
    cout << " Enter theta(0) and omega(0): ";
    double theta, omega, tMax;
    cin >> theta >> omega;
    cout << " Enter integration time t_max: ";
    cin >> tMax; */

    double dt = 0.01;
    double accuracy = 1e-6;
    ofstream dataFile("pendulum.data");

    double t = 0;
    Vector x(3);
    x[0] = t;
    x[1] = theta;
    x[2] = omega;
	double E;
	
	if (mode == 1) {
		while (t < tMax) {
			EulerStep(x, dt, f);
			t = x[0], theta = x[1], omega = x[2];
			if (nonlinear) {
				while (theta >= pi) theta -= 2 * pi;
				while (theta < -pi) theta += 2 * pi;
			}
			E = Energy(x);
			dataFile << t << '\t' << theta << '\t' << omega <<  '\t' << E << '\n';
		}
	} else if (mode == 2) {
		while (t < tMax) {
			EulerCromerStep(x, dt, f);
			t = x[0], theta = x[1], omega = x[2];
			if (nonlinear) {
				while (theta >= pi) theta -= 2 * pi;
				while (theta < -pi) theta += 2 * pi;
			}
			E = Energy(x);
			dataFile << t << '\t' << theta << '\t' << omega <<  '\t' << E << '\n';
		}
	} else if (mode == 3) {
		while (t < tMax) {
			RK4Step(x, dt, f);
			t = x[0], theta = x[1], omega = x[2];
			if (nonlinear) {
				while (theta >= pi) theta -= 2 * pi;
				while (theta < -pi) theta += 2 * pi;
			}
			E = Energy(x);
			dataFile << t << '\t' << theta << '\t' << omega <<  '\t' << E << '\n';
		}
	} else if (mode == 4) {
		while (t < tMax) {
			adaptiveRK4Step(x, dt, accuracy, f);
			t = x[0], theta = x[1], omega = x[2];
			if (nonlinear) {
				while (theta >= pi) theta -= 2 * pi;
				while (theta < -pi) theta += 2 * pi;
			}
			E = Energy(x);
			dataFile << t << '\t' << theta << '\t' << omega <<  '\t' << E << '\n';
		}
	} else {
		cout << "Rossz mode lett megadva (mode = 1 - 4 egész szám)." << endl;
	}

    /*while (t < tMax) {
        adaptiveRK4Step(x, dt, accuracy, f);
        t = x[0], theta = x[1], omega = x[2];
        if (nonlinear) {
            while (theta >= pi) theta -= 2 * pi;
            while (theta < -pi) theta += 2 * pi;
        }
        dataFile << t << '\t' << theta << '\t' << omega << '\n';
    }*/

    cout << " Output data to file pendulum.data" << endl;
    dataFile.close();
}

