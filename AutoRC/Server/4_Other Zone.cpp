#include <iostream>
#include <cmath>
#include "AutoRC.h"
using namespace std;

// ACI 319-19 APPENDIX B STEEL REINFORCEMENT INFORMATION
// ASTM STANDARD REINFORCING BARS
// Nominal diameter, in
double BarDiameter(int BarsSize) {
	switch (BarsSize) {
	case 3:
		return 0.375;
	case 4:
		return 0.500;
	case 5:
		return 0.625;
	case 6:
		return 0.750;
	case 7:
		return 0.875;
	case 8:
		return 1.000;
	case 9:
		return 1.128;
	case 10:
		return 1.270;
	case 11:
		return 1.410;
	case 14:
		return 1.693;
	case 18:
		return 2.257;
	default:
		cout << "Bars Size is Wrong!" << endl;
		return -1;
	}
}

// ACI 319-19 APPENDIX B STEEL REINFORCEMENT INFORMATION
// ASTM STANDARD REINFORCING BARS
// Nominal area, in^2
double BarArea(int BarsSize) {
	switch (BarsSize) {
	case 3:
		return 0.11;
	case 4:
		return 0.20;
	case 5:
		return 0.31;
	case 6:
		return 0.44;
	case 7:
		return 0.60;
	case 8:
		return 0.79;
	case 9:
		return 1.00;
	case 10:
		return 1.27;
	case 11:
		return 1.56;
	case 14:
		return 2.25;
	case 18:
		return 4.00;
	default:
		cout << "Bars Size is Wrong!" << endl;
		return -1;
	}
}

double ReinforceRatio(double Grade) {
	if (Grade == 60000.) {
		return 0.025;
	}
	else if (Grade == 80000.) {
		return 0.02;
	}
	else {
		cout << ">>ERROR!!!<<\nPlease make sure \"MainBarGrade\" is correct!" << endl;
		exit(0);
	}
}

// 是否為強震區
bool IsEarthquakeArea(RcData::_SDC_& SDC) {
	if (SDC == RcData::_SDC_::A || SDC == RcData::_SDC_::B || SDC == RcData::_SDC_::C) return false;
	else if (SDC == RcData::_SDC_::D || SDC == RcData::_SDC_::E || SDC == RcData::_SDC_::F) return true;
}

double CalculateDL(double Sw, double bw, double h, double hf, double Dl) {
	return (Sw * hf / 144. + bw / 12. * (h / 12. - hf / 12.)) * Dl / 1000.;
}

double CalculateLL(double Sw, double Ll) {
	return Sw * Ll / 12000.;
}