#include <iostream>
#include <cmath>
using namespace std;

double BarDiameter(int);
double BarArea(int);

double Vc(double bw, double fc, double EffectiveDepth) {
	return 2 * sqrt(fc) * bw * EffectiveDepth;
}

double StirrupForShear(double Vu, double Fyt, double d) {
	return Vu / Fyt / d / 0.75;
}

double Vs(double VuVe, double Vc, bool Ei) {
	if (Ei == true) {
		return VuVe / 0.75;
	}
	else {
		return VuVe / 0.75 - Vc;
	}
}

double VsDesign(double Av, double Fyt, double d, double Ve) {
	return 0.75 * Av * Fyt * d / Ve;
}

bool SectionSizeConfirmationForShear(double bw, double VuVe, double fc, double Vc, double EffectiveDepth) {
	return VuVe / 0.75 <= Vc + (8.0 * sqrt(fc) * bw * EffectiveDepth);
}

double Ve(double ln, double Mpr1, double Mpr2, double LoadFactorDL, double LoadFactorLL, double DL, double LL) {
	return max({ abs((Mpr1 + Mpr2) / ln + (LoadFactorDL * DL + LoadFactorLL * LL) * ln / 2.), abs((Mpr1 + Mpr2) / ln - (LoadFactorDL * DL + LoadFactorLL * LL) * ln / 2.) });
}

double AlongTheLength(int MainBarGrade, double d, int Stirrup, int MainBar) {
	double Smax{};
	if (MainBarGrade == 80) {
		Smax = min({ d / 4., 48. * BarDiameter(Stirrup), 5. * BarDiameter(MainBar), 6. });
	}
	else if (MainBarGrade == 60) {
		Smax = min({ d / 4., 48. * BarDiameter(Stirrup), 6. * BarDiameter(MainBar), 6. });
	}
	return Smax;
}

double AcrossTheWidth(double bw, double fc, double Vs, double d, int Stirrup, bool Ti) {
	if (Ti == true) {
		return min({ d / 2., 12., 48. * BarDiameter(Stirrup) });
	}
	else {
		if (Vs <= 4 * sqrt(fc) * bw * d) {
			return min({ d, 24. });
		}
		else {
			return min({ d / 2., 12. });
		}
	}
}