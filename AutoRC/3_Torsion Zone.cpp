#include <iostream>
#include <cmath>
using namespace std;

double BarDiameter(int);
double BarArea(int);
double Acp(double, double, double, double, bool);
double Pcp(double, double, double, double, bool);

double PI = 3.14159265358979;

double Acp(double bw, double h, double hf, double u, bool WingWidthType) {
	if (WingWidthType == true) {
		return bw * h + u * hf;
	}
	else {
		return bw * h + u * hf * 2.0;
	}
}

double Pcp(double bw, double h, double hf, double u, bool WingWidthType) {
	if (WingWidthType == true) {
		return 2.0 * (bw + u + h);
	}
	else {
		return 2.0 * (bw + u * 2.0 + h);
	}
}

bool GeometryConfirmation(double Tu, double bw, double hf, double h, double fc, bool WingWidthType) {
	auto u = min(h - hf, 4.0 * hf);
	double Tth = sqrt(fc) * (Acp(bw, h, hf, u, WingWidthType) * Acp(bw, h, hf, u, WingWidthType) / Pcp(bw, h, hf, u, WingWidthType));
	return 12000.0 * abs(Tu) >= 0.75 * Tth;
}

double StirrupForTorsion(double Tu, double bw, double h, double Fyt, double cc, double Stirrup) {
	double Aoh = (bw - 2.0 * cc - BarDiameter(Stirrup)) * (h - 2.0 * cc - BarDiameter(Stirrup));
	double AtCrossS = 8000.0 * abs(Tu) / (0.85 * Aoh) / Fyt * tan(45.0 * PI / 180.0);
	//cout << "At/s must >= " << AtCrossS << endl;
	return AtCrossS;
}

double TorsionBarDesignQuantity(double Tu, double bw, double h, double hf, double fc, double fy, double Fyt, double cc, double Stirrup, bool WingWidthType) {
	auto Ph = 2.0 * ((bw - 2.0 * cc - BarDiameter(Stirrup) + (h - 2.0 * cc - BarDiameter(Stirrup))));
	auto u = min(h - hf, 4.0 * hf);
	auto TestValue_1 = 5.0 * sqrt(fc) * Acp(bw, h, hf, u, WingWidthType) / fy - StirrupForTorsion(Tu, bw, h, Fyt, cc, Stirrup) * Ph * Fyt / fy;
	auto TestValue_2 = 5.0 * sqrt(fc) * Acp(bw, h, hf, u, WingWidthType) / fy - 25.0 * bw * Ph / fy;
	return min({ TestValue_1, TestValue_2 });
}

double TorsionBarSpace(double bw, double h, double cc, double Stirrup) {
	auto Ph = 2.0 * ((bw - 2.0 * cc - BarDiameter(Stirrup) + (h - 2.0 * cc - BarDiameter(Stirrup))));
	return min(Ph / 8.0, 12.0);
}