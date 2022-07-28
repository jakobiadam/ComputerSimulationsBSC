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

static double m1 = 1;  // az ingák tömegét egységnyinek választottam
static double m2 = 1;
static double L1 = 1.0;              // length of pendulum
static double q1 = 0.5;              // damping coefficient
static double Omega1_D = 2.0/3.0;    // frequency of driving force
static double F1_D = 0.9;            // amplitude of driving force
static double L2 = 1.0;              // length of pendulum
static double q2 = 0.5;              // damping coefficient
static double Omega2_D = 2.0/3.0;    // frequency of driving force
static double F2_D = 0.9;            // amplitude of driving force


Vector f(const Vector& x) {  // extended derivative vector

	// itt már egyáltalán nem alkalmazok lineáris közelítést!
    double t = x[0];
    double theta1 = x[1];
    double omega1 = x[2];
	double theta2 = x[3];
    double omega2 = x[4];
	
	double d_theta = theta1 - theta2;
	double alpha = q1 * omega1 - F1_D * sin(Omega1_D * t);
	double beta = q2 * omega2 - F2_D * sin(Omega2_D * t);
	double gamma1 = 2 * alpha - 2 * beta * cos(d_theta);
	double gamma2 = 2 * alpha * cos(d_theta) - 2 * (m1 + m2) / m2 * beta;
	
    Vector f(5);             // Vector with 5 components
    f[0] = 1;
    f[1] = omega1;
    f[2] = (m2 * L1 * pow(omega1,2) * sin(2 * d_theta) + 2 * m2 * L2* pow(omega2,2) * sin(d_theta) + 2* g * m2* cos(theta2) * sin(d_theta) + 2 * g * m1 * sin(theta1) + gamma1) /
	         (-2 * L1 * (m1 + m2 * pow(sin(d_theta),2)));
	
	f[3] = omega2;
    f[4] = (m2 * L2 * pow(omega2,2) * sin(2 * d_theta) + 2 * (m1 + m2) * L1 * pow(omega1,2) * sin(d_theta) + 2 * g * (m1 + m2) * cos(theta1) * sin(d_theta) + gamma2) /
             (2 * L2 * (m1 + m2 * pow(sin(d_theta),2)));
			 
	return f;
}
	
double Energy1(const Vector& x) {
	return m1 * g * L1 * (1 - cos(x[1])) + 1 / 2 * m1 * pow(L1 * x[2],2);
}

double Energy2(const Vector& x) {
	return m2 * g * (L1 * (1 - cos(x[1])) + L2 * (1 - cos(x[3]))) + 1 / 2 * m2 * pow(L2 * x[4],2);
}

int main(int argc, char *argv[]) {
	double theta1, omega1, theta2, omega2, tMax;
	if (argc==16) { // nem kezeltem a rossz inputokat, nem felhasználóbarát a kód.
	    m1 = atof(argv[1]);
		m2 = atof(argv[2]);
		L1 = atof(argv[3]);
		L2 = atof(argv[4]);
		q1 = atof(argv[5]);
		q2 = atof(argv[6]);
		Omega1_D = atof(argv[7]);
		Omega2_D = atof(argv[8]);
		F1_D = atof(argv[9]);
		F2_D = atof(argv[10]);
		theta1 = atof(argv[11]);
		theta2 = atof(argv[12]);
		omega1 = atof(argv[13]);
		omega2 = atof(argv[14]);
		tMax = atof(argv[15]);
		
	} else {
		cout << "15 parancssori argumentum kell, melyek rendre a következők:\n m1 \n m2 \n L1 \n L2 \n q1 \n q2 \n Omega1_D \n Omega2_D \n F1_D \n F2_D \n theta1 \n theta2\n omega1 \n omega2 \n tMax" << endl;
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
    //double accuracy = 1e-6;
    ofstream dataFile("double_pendulum.data");
	
    double t = 0;
    Vector x(5);
    x[0] = t;
    x[1] = theta1;
    x[2] = omega1;
	x[3] = theta2;
    x[4] = omega2;
	double E1;
	double E2;

	while (t < tMax) {
		RK4Step(x, dt, f);
		t = x[0], theta1 = x[1], omega1 = x[2]; theta2 = x[3]; omega2 = x[4];
		while (theta1 >= pi) theta1 -= 2 * pi;
		while (theta1 < -pi) theta1 += 2 * pi;
		while (theta2 >= pi) theta2 -= 2 * pi;
		while (theta2 < -pi) theta2 += 2 * pi;
		
		E1 = Energy1(x);
		E2 = Energy2(x);
		dataFile << t << '\t' << theta1 << '\t' << omega1 <<  '\t' << E1 << '\t' << theta2 << '\t' << omega2 <<  '\t' << E2 << '\t' << L1 * sin(theta1) << '\t' << - L1 * cos(theta1) << '\t' << L1 * sin(theta1) + L2 * sin(theta2) << '\t' << - (L1 * cos(theta1) + L2 * cos(theta2)) << '\n';
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

    cout << " Output data to file double_pendulum.data" << endl;
    dataFile.close();
}

