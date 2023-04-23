#include <cmath>
#include <vector>
#include <iostream>
using namespace std;

double BarDiameter(int);
double BarArea(int);

;

double Moment(double bwbeff, double h, double hf, double ln, double fc, double fy, double cc, double Stirrup, int MainRein, int MainReinCount, int TorsionBar, int HorizontalTorsionBarCount, int MainReinLayers, int count, bool Ei, vector <double>& EPSILON, double& COMPRESSIVESTRESSZONE) {
	double UpperBound = 0, LowerBound = h, CompressiveStressZone = h / 2, Ec = 29000000;
	double EpsilonY = fy / Ec;
	double Beta = 0.85 - 0.05 * ((fc - 4000) / 1000);
	double TotalForce;

	// Rebar Depth & Count
	vector <double> EffectiveDepthGuessC(MainReinLayers);
	EffectiveDepthGuessC[0] = cc + BarDiameter(Stirrup) + (BarDiameter(MainRein) / 2);
	EffectiveDepthGuessC[(MainReinLayers - 1)] = h - cc - BarDiameter(Stirrup) - (BarDiameter(MainRein) / 2);
	vector <int> PerLayerCount(MainReinLayers);
	PerLayerCount[0] = MainReinCount;
	PerLayerCount[(MainReinLayers - 1)] = MainReinCount;

	if (MainReinLayers == 3) {
		// 1
		EffectiveDepthGuessC[1] = EffectiveDepthGuessC[1 - 1] + 1 + BarDiameter(MainRein);
		PerLayerCount = { MainReinCount, count, MainReinCount};
		// 2
		EffectiveDepthGuessC[1] = EffectiveDepthGuessC[1 + 1] - 1 - BarDiameter(MainRein);
		PerLayerCount = { MainReinCount, count, MainReinCount };
	}
	else if (MainReinLayers == 4) {
		// 1
		EffectiveDepthGuessC[1] = EffectiveDepthGuessC[1 - 1] + 1 + BarDiameter(MainRein);
		EffectiveDepthGuessC[2] = EffectiveDepthGuessC[2 + 1] - 1 - BarDiameter(MainRein);
		PerLayerCount = { MainReinCount, MainReinCount, count, MainReinCount };
		// 2
		EffectiveDepthGuessC[1] = EffectiveDepthGuessC[1 - 1] + 1 + BarDiameter(MainRein);
		EffectiveDepthGuessC[2] = EffectiveDepthGuessC[2 + 1] - 1 - BarDiameter(MainRein);
		PerLayerCount = { MainReinCount, count, MainReinCount, MainReinCount };
		// 3
		EffectiveDepthGuessC[1] = EffectiveDepthGuessC[1 - 1] + 1 + BarDiameter(MainRein);
		EffectiveDepthGuessC[2] = EffectiveDepthGuessC[2 - 1] + 1 + BarDiameter(MainRein);
		PerLayerCount = { MainReinCount, count, MainReinCount, MainReinCount };
		// 4
		EffectiveDepthGuessC[2] = EffectiveDepthGuessC[2 + 1] - 1 - BarDiameter(MainRein);
		EffectiveDepthGuessC[1] = EffectiveDepthGuessC[1 + 1] - 1 - BarDiameter(MainRein);
		PerLayerCount = { MainReinCount, MainReinCount, count, MainReinCount };
	}
	else if (MainReinLayers == 5) {
		// 1
		EffectiveDepthGuessC[1] = EffectiveDepthGuessC[1 - 1] + 1 + BarDiameter(MainRein);
		EffectiveDepthGuessC[2] = EffectiveDepthGuessC[2 - 1] + 1 + BarDiameter(MainRein);
		EffectiveDepthGuessC[3] = EffectiveDepthGuessC[3 + 1] - 1 - BarDiameter(MainRein);
		PerLayerCount = { MainReinCount, MainReinCount, count, MainReinCount, MainReinCount };
		// 2
		EffectiveDepthGuessC[1] = EffectiveDepthGuessC[1 - 1] + 1 + BarDiameter(MainRein);
		EffectiveDepthGuessC[3] = EffectiveDepthGuessC[3 + 1] - 1 - BarDiameter(MainRein);
		EffectiveDepthGuessC[2] = EffectiveDepthGuessC[2 + 1] - 1 - BarDiameter(MainRein);
		PerLayerCount = { MainReinCount, MainReinCount, count, MainReinCount, MainReinCount };
	}
	else if (MainReinLayers == 6) {
		// 1
		EffectiveDepthGuessC[1] = EffectiveDepthGuessC[1 - 1] + 1 + BarDiameter(MainRein);
		EffectiveDepthGuessC[2] = EffectiveDepthGuessC[2 - 1] + 1 + BarDiameter(MainRein);
		EffectiveDepthGuessC[4] = EffectiveDepthGuessC[4 + 1] - 1 - BarDiameter(MainRein);
		EffectiveDepthGuessC[3] = EffectiveDepthGuessC[3 + 1] - 1 - BarDiameter(MainRein);
		PerLayerCount = { MainReinCount, MainReinCount, count, MainReinCount, MainReinCount, MainReinCount };
		// 2
		EffectiveDepthGuessC[1] = EffectiveDepthGuessC[1 - 1] + 1 + BarDiameter(MainRein);
		EffectiveDepthGuessC[2] = EffectiveDepthGuessC[2 - 1] + 1 + BarDiameter(MainRein);
		EffectiveDepthGuessC[4] = EffectiveDepthGuessC[4 + 1] - 1 - BarDiameter(MainRein);
		EffectiveDepthGuessC[3] = EffectiveDepthGuessC[3 + 1] - 1 - BarDiameter(MainRein);
		PerLayerCount = { MainReinCount, MainReinCount, MainReinCount, count, MainReinCount, MainReinCount };
	}


	//if (MainReinLayers > 2) {
	//	for (int i = 1; i < MainReinLayers / 2 - 1; i++) {
	//		EffectiveDepthGuessC[i] = EffectiveDepthGuessC[i - 1] + 1 + BarDiameter(MainRein);
	//	}
	//	for (int i = MainReinLayers - 2; i > MainReinLayers / 2; i--) {
	//		EffectiveDepthGuessC[i] = EffectiveDepthGuessC[i + 1] - 1 - BarDiameter(MainRein);
	//	}
	//}

	// Rebar Count
	//vector <int> PerLayerCount(MainReinLayers);
	//PerLayerCount[0] = MainReinCount;
	//PerLayerCount[(MainReinLayers - 1)] = MainReinCount;
	//if (MainReinLayers % 2 == 1 && MainReinLayers > 2) {
	//	PerLayerCount[int(double(MainReinLayers) / 2 + (1 / 2))] = count;
	//}
	//else if (MainReinLayers % 2 == 0 && MainReinLayers > 3) {
	//	PerLayerCount[int(-double(MainReinLayers) / 2 + 7)] = count;
	//}

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
				TotalForce += (PerLayerCount[i] * BarArea(MainRein) - HorizontalTorsionBarCount * BarArea(TorsionBar)) * Fy[i];
			}
			else {
				TotalForce += PerLayerCount[i] * BarArea(MainRein) * Fy[i];
			}
		}
		if (TotalForce >= 0) {
			LowerBound = CompressiveStressZone;
		}
		else {
			UpperBound = CompressiveStressZone;
		}
		CompressiveStressZone = (UpperBound + LowerBound) / 2;
		COMPRESSIVESTRESSZONE = CompressiveStressZone;

	} while (abs(TotalForce) > 1E-8);

	// Compute Moment
	double Moment = 0.85 * fc * pow(Beta * CompressiveStressZone, 2) * bwbeff / 2;
	for (int i = 0; i < MainReinLayers; i++) {
		if (i == 0 || i == MainReinLayers - 1) {
			Moment += (PerLayerCount[i] * BarArea(MainRein) - HorizontalTorsionBarCount * BarArea(TorsionBar)) * Fy[i] * EffectiveDepthGuessC[i];
		}
		else {
			Moment += PerLayerCount[i] * BarArea(MainRein) * Fy[i] * EffectiveDepthGuessC[i];
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