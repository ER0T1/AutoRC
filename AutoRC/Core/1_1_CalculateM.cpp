#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

double BarDiameter(int);
double BarArea(int);

double Moment(double bwbeff, double h, double fc, double fy, int MainBar, vector <double> EffectiveDepthGuessC, vector <int> PerLayerCount,int TorsionBar, int HorizontalTorsionBarCount, bool Ei, vector <double>& EPSILON, double& COMPRESSIVESTRESSZONE) {
	double UpperBound = 0, LowerBound = h, CompressiveStressZone = h / 2, Ec = 29000000;
	double EpsilonY = fy / Ec;
	double Beta = 0.85 - 0.05 * ((fc - 4000) / 1000);
	double TotalForce;

	cout << "d4a" << endl;
	// Guess C
	// vector <double> Epsilon(PerLayerCount.size());
	EPSILON.resize(PerLayerCount.size());
	vector <double> Fy(PerLayerCount.size());
	do {
		TotalForce = 0.85 * fc * Beta * CompressiveStressZone * bwbeff;
		for (int i = 0; i < PerLayerCount.size(); i++) {
			EPSILON[i] = 0.003 - 0.003 / CompressiveStressZone * EffectiveDepthGuessC[i];
			if (abs(EPSILON[i]) >= EpsilonY) {
				if (EPSILON[i] >= 0) {
					if (Ei == true) {
						Fy[i] = 1.25 * fy;
					}
					else {
						Fy[i] = fy;
					}
				}
				else {
					if (Ei == true) {
						Fy[i] = -1.25 * fy;
					}
					else {
						Fy[i] = -fy;
					}
				}
			}
			else {
				Fy[i] = EPSILON[i] * Ec;
			}
			if (i == 0 || i == PerLayerCount.size() - 1) {
				TotalForce += (PerLayerCount[i] * BarArea(MainBar) - HorizontalTorsionBarCount * BarArea(TorsionBar)) * Fy[i];
				cout << "++" << TotalForce << "Low" << LowerBound << "Up" << UpperBound << endl;
			}
			else {
				TotalForce += PerLayerCount[i] * BarArea(MainBar) * Fy[i];
				cout << "--" << TotalForce << "Low" << LowerBound << "Up" << UpperBound << endl;
			}
		}
		if (TotalForce > 0) {
			LowerBound = CompressiveStressZone;
		}
		else if (TotalForce < 0) {
			UpperBound = CompressiveStressZone;
		}
		CompressiveStressZone = (UpperBound + LowerBound) / 2.;
		COMPRESSIVESTRESSZONE = CompressiveStressZone;
		cout << "d4b" << endl;
	} while (abs(TotalForce) > 1E-8);
	cout << TotalForce << endl;
	cout << "d4c" << endl;
	// Compute Moment
	double Moment = 0.85 * fc * pow(Beta * CompressiveStressZone, 2) * bwbeff / 2;
	for (int i = 0; i < PerLayerCount.size(); i++) {
		if (i == 0 || i == PerLayerCount.size() - 1) {
			Moment += (PerLayerCount[i] * BarArea(MainBar) - HorizontalTorsionBarCount * BarArea(TorsionBar)) * Fy[i] * EffectiveDepthGuessC[i];
		}
		else {
			Moment += PerLayerCount[i] * BarArea(MainBar) * Fy[i] * EffectiveDepthGuessC[i];
		}
	}
	cout << "d4d" << endl;
	return -Moment;
}

double ReductionFactorMforOther(double fy, vector <double>& EPSILON) {
	if (abs(EPSILON[EPSILON.size() - 1]) <= fy / 29000000) {
		return 0.65;
	}
	else if (abs(EPSILON[EPSILON.size() - 1]) >= fy / 29000000 + 0.003) {
		return 0.9;
	}
	else {
		return 0.65 + (abs(EPSILON[EPSILON.size() - 1]) - (3. / 1450)) * (0.25 / 0.003);
	}
}

double ReductionFactorMforSpiral(double fy, vector <double>& EPSILON) {
	if (abs(EPSILON[EPSILON.size() - 1]) <= fy / 29000000) {
		return 0.75;
	}
	else if (abs(EPSILON[EPSILON.size() - 1]) >= fy / 29000000 + 0.003) {
		return 0.9;
	}
	else {
		return 0.75 + (abs(EPSILON[EPSILON.size() - 1]) - (3. / 1450)) * (0.15 / 0.003);
	}
}