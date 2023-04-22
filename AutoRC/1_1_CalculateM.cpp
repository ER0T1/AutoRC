#include <cmath>
#include <vector>
#include <iostream>
using namespace std;

double BarDiameter(int);
double BarArea(int);

;

double Moment(double bwbeff, double h, double hf, double ln, double fc, double fy, double cc, double Stirrup, int MainRein, int MainReinCount, int TorsionBar, int HorizontalTorsionBarCount, int MainReinLayers, bool Ei, vector <double> &EPSILON) {
	double UpperBound = 0, LowerBound = h, CompressiveStressZone = h / 2, Ec = 29000000;
	double EpsilonY = fy / Ec;
	double Beta = 0.85 - 0.05 * ((fc - 4000) / 1000);
	double TotalForce;

	// Rebar Depth
	vector <double> EffectiveDepthGuessC(MainReinLayers);
	EffectiveDepthGuessC[0] = cc + BarDiameter(Stirrup) + (BarDiameter(MainRein) / 2);
	EffectiveDepthGuessC[(MainReinLayers - 1)] = h - cc - BarDiameter(Stirrup) - (BarDiameter(MainRein) / 2);
	if (MainReinLayers > 2) {
		for (int i = 1; i < MainReinLayers / 2 - 1; i++) {
			EffectiveDepthGuessC[i] = EffectiveDepthGuessC[i - 1] + 1 + BarDiameter(MainRein);
		}
		for (int i = MainReinLayers - 2; i > MainReinLayers / 2; i--) {
			EffectiveDepthGuessC[i] = EffectiveDepthGuessC[i + 1] - 1 - BarDiameter(MainRein);
		}
	}

	// Guess C
	vector <double> Epsilon(MainReinLayers);
	vector <double> Fy(MainReinLayers);
	do {
		TotalForce = 0.85 * fc * Beta * CompressiveStressZone * bwbeff;
		for (int i = 0; i < MainReinLayers; i++) {
			Epsilon[i] = 0.003 - 0.003 / CompressiveStressZone * EffectiveDepthGuessC[i];
			EPSILON[i] = Epsilon[i];
			if (abs(Epsilon[i]) >= EpsilonY) {
				if (Epsilon[i] >= 0) {
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
				Fy[i] = Epsilon[i] * Ec;
			}
			if (i == 0 || i == MainReinLayers - 1) {
				TotalForce += (MainReinCount * BarArea(MainRein) - HorizontalTorsionBarCount * BarArea(TorsionBar)) * Fy[i];
			}
			else {
				TotalForce += MainReinCount * BarArea(MainRein) * Fy[i];
			}
		}
		if (TotalForce >= 0) {
			LowerBound = CompressiveStressZone;
		}
		else {
			UpperBound = CompressiveStressZone;
		}
		CompressiveStressZone = (UpperBound + LowerBound) / 2;

	} while (abs(TotalForce) > 1E-8);

	// Compute Moment
	double Moment = 0.85 * fc * pow(Beta * CompressiveStressZone, 2) * bwbeff / 2;
	for (int i = 0; i < MainReinLayers; i++) {
		if (i == 0 || i == MainReinLayers - 1) {
			Moment += (MainReinCount * BarArea(MainRein) - HorizontalTorsionBarCount * BarArea(TorsionBar)) * Fy[i] * EffectiveDepthGuessC[i];
		}
		else {
			Moment += MainReinCount * BarArea(MainRein) * Fy[i] * EffectiveDepthGuessC[i];
		}
	}
	return -Moment;
}

double ReductionFactorMforOther(int MainReinLayers, double fy, vector <double> &EPSILON) {
	if (abs(EPSILON[MainReinLayers - 1]) <= fy / 29000000) {
		return 0.65;
	}
	else if (abs(EPSILON[MainReinLayers - 1]) >= fy / 29000000 + 0.003) {
		return 0.9;
	}
	else {
		return 0.65 + (abs(EPSILON[MainReinLayers - 1]) - (3. / 1450)) * (0.25 / 0.003);
	}
}

double ReductionFactorMforSpiral(int MainReinLayers, double fy, vector <double>& EPSILON) {
	if (abs(EPSILON[MainReinLayers - 1]) <= fy / 29000000) {
		return 0.75;
	}
	else if (abs(EPSILON[MainReinLayers - 1]) >= fy / 29000000 + 0.003) {
		return 0.9;
	}
	else {
		return 0.75 + (abs(EPSILON[MainReinLayers - 1]) - (3. / 1450)) * (0.15 / 0.003);
	}
}