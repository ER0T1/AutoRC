#include <iostream>
#include <cmath>
using namespace std;

double BarDiameter(int);
double BarArea(int);

// Global Variable
double PI = 3.14159265358979;

double Acp(double bw, double h, double hf, bool WingWidthType) {
	auto u = min(h - hf, 4.0 * hf);
	if (WingWidthType == true) {
		return bw * h + u * hf;
	}
	else {
		return bw * h + u * hf * 2.0;
	}
}

double Pcp(double bw, double h, double hf, bool WingWidthType) {
	auto u = min(h - hf, 4.0 * hf);
	if (WingWidthType == true) {
		return 2.0 * (bw + u + h);
	}
	else {
		return 2.0 * (bw + u * 2.0 + h);
	}
}

double Ph(double bw, double h, double cc, double Stirrup) {
	return 2.0 * ((bw - 2.0 * cc - BarDiameter(Stirrup) + (h - 2.0 * cc - BarDiameter(Stirrup))));
}

double Aoh(double bw, double h, double cc, int Stirrup) {
	return (bw - 2.0 * cc - BarDiameter(Stirrup)) * (h - 2.0 * cc - BarDiameter(Stirrup));
}

bool GeometryConfirmation(double Tu, double fc, double Acp, double Pcp) {
	auto Tth = sqrt(fc) * (Acp / Pcp);
	return 12000.0 * abs(Tu) >= 0.75 * Tth;
}

double StirrupForTorsion(double Tu, double Fyt, double Aoh) {
	return 12.0 * abs(Tu) / 2 / (0.85 * Aoh) / Fyt;
}

double TorsionBarDesignQuantity(double Tu, double bw, double fc, double fy, double Fyt, double StirrupForTorsion, double Ph, double Acp, double Aoh) {
	auto TestValue_1 = 5.0 * sqrt(fc) * Acp / fy - StirrupForTorsion * Ph * Fyt / fy;
	auto TestValue_2 = 5.0 * sqrt(fc) * Acp / fy - 25.0 * bw * Ph / fy;
	auto Tast_1 = min({ TestValue_1, TestValue_2 });
	auto Test_2 = Tu * Ph * 12 / 1.275 / Aoh / Fyt;
	return max({ Tast_1, Test_2 });
}

double TorsionBarSpace(double Ph, int MainBar, int Stirrup, int TorsionBar) {
	return min({ Ph / 8.0, 12.0, 16 * BarDiameter(MainBar), 48 * BarDiameter(Stirrup),24 * BarDiameter(TorsionBar) });
	// return min({ Ph / 8.0, 12.0 });
}

bool SectionSizeConfirmationForTorsion(double bw, double Vu, double Tu, double fc, double EffectiveDepth, double Ph, double Aoh, double Vc) {
	// 目前只有soild，尚未建立hollow
	auto TestValue_1 = sqrt(pow(Vu / (bw * EffectiveDepth), 2) + pow((Tu * Ph / (1.7 * pow(Aoh, 2))), 2));
	auto TestValue_2 = 0.75 * (Vc / bw * EffectiveDepth + 8 * sqrt(fc));
	return TestValue_1 <= TestValue_2;
}

double TorsionBarDesignStrength(double fy,double Aoh, double StirrupForTorsion, double Fyt, double Ph, int TorsionBarCount, int TorsionBar) {
	auto TestValue_1 = 2 * 0.85 * Aoh * StirrupForTorsion * Fyt;
	auto TestValue_2 = 2 * 0.85 * Aoh * TorsionBarCount * BarArea(TorsionBar) * fy / Ph;
	return min({ TestValue_1, TestValue_2 });
}