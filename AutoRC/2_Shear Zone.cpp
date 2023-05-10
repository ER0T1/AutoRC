#include <iostream>
#include <cmath>
using namespace std;

double BarDiameter(int);
double BarArea(int);

double Vc(double bw, double fc, double EffectiveDepth) {
	return 2 * sqrt(fc) * bw * EffectiveDepth;
}

bool SectionSizeConfirmationForShear(double bw, double Vu, double fc, double Vc, double EffectiveDepth) {
	return Vu / 0.75 <= Vc + (8.0 * sqrt(fc) * bw * EffectiveDepth);
}