#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <unistd.h>
using namespace std;

double omega;          // the natural frequency
double x, v;           // position and velocity at time t
double x2, v2, x3, v3, x0, v0;
double E, E2, E3;
int periods;           // number of periods to integrate
int stepsPerPeriod;    // number of time steps dt per period
string fileName;       // name of output file

void getInput();                 // for user to input parameters
void EulerCromer(double dt);     // takes an Euler-Cromer step
void Euler(double dt);
void Analytic(double dt);
double energy(double x, double v);                 // computes the energy

void getInput ( ) {
    cout << "Enter omega: ";
    cin >> omega;
    cout << "Enter x(0) and v(0): ";
    cin >> x >> v;
    x2 = x; v2 = v; x3 = x; v3 = v; x0 = x; v0 = v;
    cout << "Enter number of periods: ";
    cin >> periods;
    cout << "Enter steps per period: ";
    cin >> stepsPerPeriod;
    cout << "Enter output file name: ";
    cin >> fileName;
}

void EulerCromer (double dt) {
    double a = - omega * omega * x;
    v += a * dt;
    x += v * dt;
    E = energy(x, v);
}

void Euler (double dt){
    double a2 = - omega * omega * x2;
    x2 += v2*dt;
    v2 += a2*dt;
    E2 = energy(x2, v2);
}
void Analytic(double t){
    x3 = x0*cos(omega*t) + (v0 / omega) * sin(omega*t);
    v3 = -x0*sin(omega*t) * omega + v0 *cos(omega*t);
    E3 = energy(x3, v3);
}

double energy (double x, double v) {
    return 0.5 * (v * v + omega * omega * x * x);
}

int main (int argc, char** argv) {
	auto start = chrono::steady_clock::now();

    if (argc<7) {
        return -1;
    }

    omega=atof(argv[1]);          // the natural frequency
    x=atof(argv[2]);
    v=atof(argv[3]);           // position and velocity at time t
    periods=atof(argv[4]);           // number of periods to integrate
    stepsPerPeriod=atof(argv[5]);    // number of time steps dt per period
    fileName=argv[6];
    x2 = x; v2 = v; x3 = x; v3 = v; x0 = x; v0 = v;
    E = energy(x, v);
    E2 = energy(x, v);
    E3 = energy(x, v );
    //getInput();
    ofstream file(fileName.c_str());
    if (!file) {
        cerr << "Cannot open " << fileName << "\nExiting ...\n";
        return 1;
    }
    const double pi = 4 * atan(1.0);
    double T = 2 * pi / omega;
    double dt = T / stepsPerPeriod;
    double t = 0;
    file << t << '\t' << x0 << '\t' << v0 << '\t' << E << '\t' << x0 << '\t' << v0 << '\t' << E << '\t' << x0 << '\t' << v0 << '\t' << E <<'\n';
    for (int p = 1; p <= periods; p++) {
        for (int s = 0; s < stepsPerPeriod; s++) {
            EulerCromer(dt);
            Euler(dt);
            t += dt;
	    Analytic(t);
	    file << t << '\t' << x << '\t' << v << '\t' << E << '\t' << x2 << '\t' << v2 << '\t' << E2 << '\t' << x3 << '\t' << v3 << '\t' << E3 << '\n';
        }
    }
    file.close();
	auto end = chrono::steady_clock::now();
	
	cout 	<< chrono::duration_cast<chrono::microseconds>(end - start).count()
		<< " ??s" << endl;
}

