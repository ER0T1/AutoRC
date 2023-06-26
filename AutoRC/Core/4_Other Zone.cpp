#include <iostream>
#include <cmath>
using namespace std;

double BarDiameter(int BarsSize) {        // in
	switch (BarsSize) {
	case 3:
		return 0.375;
	case 4:
		return 0.5;
	case 5:
		return 0.625;
	case 6:
		return 0.750;
	case 7:
		return 0.875;
	case 8:
		return 1;
	case 9:
		return 1.128;
	case 10:
		return 1.27;
	default:
		cout << "Bars Size is Wrong!" << endl;
		return -1;
	}
}

double BarArea(int BarsSize) {        // in^2
	switch (BarsSize) {
	case 3:
		return 0.11;
	case 4:
		return 0.2;
	case 5:
		return 0.31;
	case 6:
		return 0.44;
	case 7:
		return 0.6;
	case 8:
		return 0.79;
	case 9:
		return 1;
	case 10:
		return 1.27;
	default:
		cout << "Bars Size is Wrong!" << endl;
		return -1;
	}
}

double ReinforceRatio(int Grade) {
	if (Grade == 60) {
		return 0.025;
	}
	else if (Grade == 80) {
		return 0.02;
	}
	else {
		cout << ">>ERROR!!!<<\nPlease make sure \"MainReinGrade\" is correct!" << endl;
		exit(0);
	}
}

double CalculateDL(double Sw, double bw, double h, double hf, double Dl) {
	return (Sw * hf / 144. + bw / 12. * (h / 12. - hf / 12.)) * Dl / 1000.;
}

double CalculateLL(double Sw, double Ll) {
	return Sw * Ll / 12000.;
}