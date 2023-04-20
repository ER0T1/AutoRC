#include <iostream>
#include < fstream >
#include <math.h>
using namespace std;

// Moment Zone
double EffectiveDepth(double, double, double);
double EffectiveWingWidth(double, double, double, double, bool);
double As(double, double, double, double);
double AsMinValue(double, double, double, double);
bool AsMin(double, double);
double AsMaxValue(double, double, double);
bool AsMax(double, double, double, double);
double Grade(int);
double MaxBarSpacing(double, int);
double MinBarSpacing(double, int);
double GuessC(double, double, double, double, double, double, double, double, double, int, int, int, bool);

// Shear Zone
double StirrupForShear(double, double, double, double, double);

// Torsion Zone
double TuConfirmation(double, double, double, double, double, double, double, double, bool, int);
bool GeometryConfirmation(double, double, double, double, double, bool);
bool SizeConfirmation(double, double, double, double, double, double, double, bool);
double StirrupForTorsion(double, double, double, double, double);
double TorqueBarDesignQuantity(double, double, double, double, double, double, double, double, bool);
double AlongTheLength(double, double, int, int);
double AcrossTheWidth(double, double, int, int, int);

// Other Zone
double BarDiameter(int);
double BarArea(int);

// Global Variable
int MainReinGrade = 60;
int StirrupGrade = 60;
int TorsionBarGrade = 60;
double dagg = 3 / 4;
double PI = 3.14159265358979;

int main() {
	// Read Strength Information:
	double Mu = 1, Vu = 1, Tu = 1; // klb-ft, klb, klb-ft
	// Read Size Information:
	double Sw = 200, bw = 15, hf = 5, h = 20, ln = 200; // in
	// Read Material Information:
	double fc = 5000, fy = 60000; // psi

	double fyt = 60;


	/*double* Information;
	Information = new double[10];
	for (int i = 0; i < 10; i++) {
		ifstream Information[i] = ifstream("Information.txt");
	}*/


	// bw, h Confirmation
	if (bw < 0.3 * h || bw < 10) {
		cout << "Your beam has insufficient section dimensions, please increase the beam width!" << endl;
		exit(0);
	}
	bool SkinRebar = false;
	if (h >= 36) {
		SkinRebar = true;
	}

	// Wing Width Type Confirmation
	cout << "Whether it is Single Wing or Double Wing? (S/D)" << endl;
	bool WingWidthType;
	char WidthType[1];
	while (true) {
		cin >> WidthType;
		if (WidthType == "S") {
			cout << "It is Single Wing." << endl;
			WingWidthType = true;
			break;
		}
		else if (WidthType == "D") {
			cout << "It is Double Wing." << endl;
			WingWidthType = false;
			break;
		}
		else {
			cout << "Please answer correctly!\nWhether it is Single Wing or Double Wing? (S/D)" << endl;
		}
	}

	//Strong Earthquake Area Confirmation
	bool Ei;
	char EarthquakeArea[3];
	cout << "Is it designed for strong earthquake areas? (Y/N)" << endl;
	while (true) {
		cin >> EarthquakeArea;
		if (EarthquakeArea == "Y" || EarthquakeArea == "y" || EarthquakeArea == "Yes" || EarthquakeArea == "yes" || EarthquakeArea == "是") {
			Ei = true;
			break;
		}
		else if (EarthquakeArea == "N" || EarthquakeArea == "n" || EarthquakeArea == "No" || EarthquakeArea == "no" || EarthquakeArea == "否") {
			Ei = false;
			break;
		}
		else {
			cout << "Please answer correctly!\nIs it designed for strong earthquake areas? (Y/N)" << endl;
		}
	}

	/*

	// Geometry Confirmation ( Confirm whether torsion bars are required. )
	if (GeometryConfirmation(Tu, bw, hf, h, fc, WingWidthType) == true) {
		// Reinforcement ( Confirm whether there is a strong earthquake area. )
		// 固定鋼筋保護層1.5英吋，箍筋從#4開始，扭矩筋從#3開始，主筋從#6開始
		for (int MainRein = 5; MainRein <= 10; MainRein++) {
			for (int Stirrup = 4; (Stirrup < MainRein || Stirrup <= 6); Stirrup++) {
				for (int TorsionBar = 3; (TorsionBar <= MainRein || TorsionBar <= 7); TorsionBar++) {

					// 計算扭矩筋支數
					int TorsionBarCount = TorqueBarDesignQuantity(bw, h, hf, fc, fy, fyt, Stirrup, StirrupForTorsion(Tu, bw, h, fyt, Stirrup), WingWidthType) / BarArea(TorsionBar) + 1;
					if (TorsionBarCount % 2 == 0) {
						TorsionBarCount += 2;
					}
					else {
						TorsionBarCount += 1;
					}
					int HorizontalTorsionBarCount = (bw - 3 - 2 * BarDiameter(Stirrup) + AcrossTheWidth(bw, h, MainRein, Stirrup, TorsionBar)) / (BarDiameter(TorsionBar) + AcrossTheWidth(bw, h, MainRein, Stirrup, TorsionBar)) + 1;
					int VerticalTorsionBarCount = (h - 3 - 2 * BarDiameter(Stirrup) + AlongTheLength(bw, h, MainRein, Stirrup)) / (BarDiameter(TorsionBar) + AlongTheLength(bw, h, MainRein, Stirrup)) + 1;
					while (true) {
						if (TorsionBarCount > 2 * (HorizontalTorsionBarCount + VerticalTorsionBarCount) - 4) {
							HorizontalTorsionBarCount += 1;
							VerticalTorsionBarCount += 1;
						}
						else {
							break;
						}
					}

					int VerticalMainReinCount = (h - 3 - 2 * BarDiameter(Stirrup) + 1) / (BarDiameter(MainRein) + 1);
					int TemporaryValue = VerticalMainReinCount / VerticalTorsionBarCount;
					int MaxMainReinLayers = VerticalTorsionBarCount * TemporaryValue;

					for (int MainReinCount = AsMinValue(bw, fc, fy, EffectiveDepth(h, Stirrup, MainRein)) / BarArea(MainRein); MainReinCount <= AsMaxValue(bw, EffectiveDepth(h, Stirrup, MainRein), Grade(MainReinGrade)) / BarArea(MainRein); MainReinCount++) {
						for (int MainReinLayers = 2; MainReinLayers <= MaxMainReinLayers; MainReinLayers += 2) { // 目前只能兩層
							
							// Is the section large enough?
							// Stirrup for shear
							// Stirrup for torsion
							// Design stirrup for shear & torsion
							// Check spacing
							// Design for longtitudinal reinforcement
							cout << "Main Rein = #" << MainRein << " Stirrup = #" << Stirrup << " TorsionBar = #" << TorsionBar << endl;


						}
					}
				}
			}
		}
	}
	else if (GeometryConfirmation(Tu, bw, hf, h, fc, WingWidthType) == false) {
		// Reinforcement ( Confirm whether there is a strong earthquake area. )
	}

	return 0;

	*/

}

// Moment Zone
/*
double EffectiveDepth(double h, double Stirrup, double MainRein) {
	double d = h - 1.5 - BarDiameter(Stirrup) - (BarDiameter(MainRein) / 2);
	return d;
}

double EffectiveWingWidth(double bw, double Sw, double hf, double ln, bool WingWidthType) {
	double beff;
	if (WingWidthType == true) {
		beff = bw + min({ Sw / 2.0, 6.0 * hf, ln / 12.0 });
	}
	else {
		beff = bw + min({ Sw / 2.0, 8.0 * hf, ln / 12.0 });
	}
	return beff;
}

double As(double MainRein, double MainReinCount, double TorsionBar, double TorsionBarCount) {
	double As = BarArea(MainRein) * MainReinCount - BarArea(TorsionBar) * TorsionBarCount;
	return As;
}

double AsMinValue(double bw, double fc, double fy, double d) {
	double As1 = bw * d * 3 * sqrt(fc) / fy;
	double As2 = bw * d * 200 / fy;
	double AsMin = max({ As1, As2 });
	return AsMin;
}

bool AsMin(double As, double AsMinValue) {
	if (As >= AsMinValue) {
		return true;
	}
	else {
		return false;
	}
}

double AsMaxValue(double bw, double EffectiveDepth, double Grade) {
	double AsMaxValue = bw * EffectiveDepth * Grade;
	return AsMaxValue;
}

bool AsMax(double bw, double As, double d, double Grade) {
	if (As / (bw * d) <= Grade) {
		return true;
	}
	else {
		return false;
	}
}

double Grade(int MainReinGrade) {
	if (MainReinGrade == 60) {
		double Grade = 0.025;
		return Grade;
	}
	else if (MainReinGrade == 80) {
		double Grade = 0.02;
		return Grade;
	}
	else {
		cout << ">>ERROR!!!<<\nPlease make sure \"MainReinGrade\" is correct!" << endl;
		exit(0);
	}
}

double MaxBarSpacing(double fy, int Stirrup) {
	double MaxBarSpacing = min({ 900000.0 / fy - 2.5 * (1.5 + BarDiameter(Stirrup)), 720000.0 / fy });
	return MaxBarSpacing;
}

double MinBarSpacing(double dagg, int MainRein) {
	double MinBarSpacing = max({ 4 / 3 * dagg, BarDiameter(MainRein), 1.0 });
	return MinBarSpacing;
}

double GuessC(double Sw, double bw, double h, double hf, double ln, double fc, double fy, double Stirrup, double beff, int MainRein, int MainReinCount, int MainReinLayers, bool Ei) {
	double UpperBound = 0, LowerBound = h, CompressiveStressZone = h / 2, Ec = 29000;
	double EpsilonY = fy / Ec;
	double Beta = 0.85 - 0.05 * ((fc - 4000) / 1000);
	double TotalForce = 0.85 * fc * Beta * CompressiveStressZone * bw;

	// Rebar Depth
	double* EffectiveDepthGuessC;
	EffectiveDepthGuessC = new double[MainReinLayers];
	EffectiveDepthGuessC[0] = 1.5 + BarDiameter(Stirrup) + (BarDiameter(MainRein) / 2);
	EffectiveDepthGuessC[(MainReinLayers-1)] = h - 1.5 - BarDiameter(Stirrup) - (BarDiameter(MainRein) / 2);
	for (int i = 1; i < (MainReinLayers - 1); i++) {
		EffectiveDepthGuessC[i] = EffectiveDepthGuessC[(i - 1)] + ((EffectiveDepthGuessC[(MainReinLayers - 1)] - EffectiveDepthGuessC[0]) / (MainReinLayers - 1));
	}

	// Main Program
	int* Epsilon;
	Epsilon = new int[MainReinLayers];
	int* Fy;
	Fy = new int[MainReinLayers];
	do {
		for (int i = 0; i < MainReinLayers; i++) {
			Epsilon[i] = 0.003 - 0.003 / CompressiveStressZone * EffectiveDepthGuessC[i];
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
			TotalForce += MainReinCount * BarArea(MainRein) * Fy[i];
		}
		if (TotalForce >= 0) {
			LowerBound = CompressiveStressZone;
		}
		else {
			UpperBound = CompressiveStressZone;
		}
		CompressiveStressZone = (UpperBound + LowerBound) / 2;

	} while (abs(TotalForce) > 0.000001);
	if (CompressiveStressZone > hf) {
		cout << "The depth of Compressive Stress Zone has exceeded the thickness of the slab." << endl;
	}
	return CompressiveStressZone;
}

//double Mn(double beffbw, double h, double fc, double fy, int MainReinLayers, int MainRein, int Stirrup, double CompressiveStressZone  ) {
//	double Beta = 0.85 - 0.05 * ((fc - 4000) / 1000);
//	double Mn = 0.85 * fc * pow(Beta * CompressiveStressZone, 2) * beffbw / 2;
//
//	// Rebar Depth
//	double* EffectiveDepthGuessC;
//	EffectiveDepthGuessC = new double[MainReinLayers];
//	EffectiveDepthGuessC[0] = 1.5 + BarDiameter(Stirrup) + (BarDiameter(MainRein) / 2);
//	EffectiveDepthGuessC[(MainReinLayers - 1)] = h - 1.5 - BarDiameter(Stirrup) - (BarDiameter(MainRein) / 2);
//	for (int i = 1; i < (MainReinLayers - 1); i++) {
//		EffectiveDepthGuessC[i] = EffectiveDepthGuessC[(i - 1)] + ((EffectiveDepthGuessC[(MainReinLayers - 1)] - EffectiveDepthGuessC[0]) / (MainReinLayers - 1));
//	}
//
//
//	
//	return 0;
//}

// Shear Zone
double StirrupForShear(double Vu, double bw, double fc, double fy, double d) {
	double Vs = (500 * Vu) - 2 * sqrt(fc) * bw * d;
	double AvCrossS = Vs / fy / d;
	cout << "Av/s must >= " << AvCrossS << endl;
	return AvCrossS;
}

// Torsion Zone
double TuConfirmation(double Tu, double bw, double hf, double h, double fc, double fy, double fyt, double AtCrossS, bool WingWidthType, int Stirrup) {
	double Aoh = (bw - 3 - BarDiameter(Stirrup) * (h - 3 - BarDiameter(Stirrup)));
	double Ph = 2 * ((bw - 3 - BarDiameter(Stirrup) + (h - 3 - BarDiameter(Stirrup))));
	double TestValue_1 = 1.7 * Aoh * StirrupForTorsion(Tu, bw, h, fyt, Stirrup) * fyt / tan(45 * PI / 180);
	double TestValue_2 = 1.7 * Aoh * TorqueBarDesignQuantity(bw, h, hf, fc, fy, fyt, Stirrup, AtCrossS, WingWidthType) * fy / Ph / tan(45 * PI / 180);
	double Tn = min({ TestValue_1, TestValue_2 });
	double Acp;
	double Pcp;
	double u = min({ h - hf, 4 * hf });

	if (0.75 * Tn >= Tu) {
		if (WingWidthType == true) {
			Acp = bw * h + u * hf;
			Pcp = 2 * (bw + u + h);
		}
		else {
			Acp = bw * h + u * hf * 2;
			Pcp = 2 * (bw + u * 2 + h);
		}
	}
	double Tu2 = 4 * sqrt(fc) * (Acp * Acp / Pcp);
	double TuConfirmation = min({ Tu, Tu2 });
	return TuConfirmation;
	}

*/


//bool GeometryConfirmation(double Tu, double bw, double hf, double h, double fc, bool WingWidthType) {
//	double Tth;
//	double u = min({ h - hf, 4.0 * hf });
//	if (WingWidthType == true) {
//		double Acp = bw * h + u * hf;
//		double Pcp = 2.0 * (bw + u + h);
//		Tth = sqrt(fc) * (Acp * Acp / Pcp);
//	}
//	else {
//		double Acp = bw * h + u * hf * 2.0;
//		double Pcp = 2.0 * (bw + u * 2.0 + h);
//		Tth = sqrt(fc) * (Acp * Acp / Pcp);
//	}
//
//	if (12000.0 * Tu >= 0.75 * Tth) {
//		return true;
//	}
//	else {
//		return false;
//	}
//}


/*

bool SizeConfirmation(double Vu, double Tu, double bw, double h, double fc, double Stirrup, double d, bool Ei) {
	double Aoh = (bw - 3 - BarDiameter(Stirrup) * (h - 3 - BarDiameter(Stirrup)));
	double Ph = 2 * ((bw - 3 - BarDiameter(Stirrup) + (h - 3 - BarDiameter(Stirrup))));
	double Vc = 2 * sqrt(fc) * bw * d;
	if (Ei == true) {
		Vc = 0;
	}
	
	double TestValue = sqrt(pow((1000 * Vu / (bw * d)), 2) + pow(((12000 * Tu * Ph) / (1.7 * Aoh * Aoh)), 2));
	if (TestValue > 0.75 * ((Vc / (bw * d)) + 8 * sqrt(fc))) {
		cout << "The section size of beam is insufficient!" << endl;
		exit(0);
		return false;
	}
	cout << "The section size of beam is sufficient!" << endl;
	return true;
}

*/
//double StirrupForTorsion(double Tu, double bw, double h, double fyt, double Stirrup) {
//	double Aoh = (bw - 3.0 - BarDiameter(Stirrup) * (h - 3.0 - BarDiameter(Stirrup)));
//	double AtCrossS = 8000.0 * Tu / (0.85 * Aoh) / fyt * tan(45.0*PI/180.0);
//	cout << "At/s must >= " << AtCrossS << endl;
//	return AtCrossS;
//}
//
//double TorqueBarDesignQuantity(double bw, double h, double hf, double fc, double fy, double fyt, double Stirrup, double AtCrossS, bool WingWidthType) {
//	double Aoh = (bw - 3.0 - BarDiameter(Stirrup) * (h - 3.0 - BarDiameter(Stirrup)));
//	double Ph = 2.0 * ((bw - 3.0 - BarDiameter(Stirrup) + (h - 3.0 - BarDiameter(Stirrup))));
//	double u = min({ h - hf, 4.0 * hf });
//	double Acp, Pcp;
//	if (WingWidthType == true) {
//		Acp = bw * h + u * hf;
//		Pcp = 2.0 * (bw + u + h);
//	}
//	else {
//		Acp = bw * h + u * hf * 2.0;
//		Pcp = 2.0 * (bw + u * 2.0 + h);
//	}
//
//	double TestValue_1 = 5.0 * sqrt(fc) * Acp / fy - AtCrossS * Ph * fyt / fy;
//	double TestValue_2 = 5.0 * sqrt(fc) * Acp / fy - 25.0 * bw * Ph / fy;
//	double Al = min({ TestValue_1, TestValue_2 });
//	return Al;
//}
/*

double AlongTheLength(double bw, double h,int MainRein, int Stirrup) {
	double Ph = 2 * ((bw - 3 - BarDiameter(Stirrup) + (h - 3 - BarDiameter(Stirrup))));
	double EffectiveDepthA = h - 1.5 - BarDiameter(Stirrup) - BarDiameter(MainRein) / 2;
	double S = min({ EffectiveDepthA / 2, Ph / 8, 12.0 });
	return S;
}

double AcrossTheWidth(double bw, double h, int MainRein, int Stirrup, int TorsionBar) {
	double Ph = 2 * ((bw - 3 - BarDiameter(Stirrup) + (h - 3 - BarDiameter(Stirrup))));
	double EffectiveDepthA = h - 1.5 - BarDiameter(Stirrup) - BarDiameter(MainRein) / 2;
	double S = min({ EffectiveDepthA, 48 * BarDiameter(TorsionBar), Ph / 8, 12.0 });
	return S;
}

*/

// Other Zone
//double BarDiameter(int BarsSize) {        // in
//	double BarDiameter;
//	switch (BarsSize) {
//		case 3:
//			BarDiameter = 0.375;
//			break;
//		case 4:
//			BarDiameter = 0.5;
//			break;
//		case 5:
//			BarDiameter = 0.625;
//			break;
//		case 6:
//			BarDiameter = 0.750;
//			break;
//		case 7:
//			BarDiameter = 0.875;
//			break;
//		case 8:
//			BarDiameter = 1;
//			break;
//		case 9:
//			BarDiameter = 1.128;
//			break;
//		case 10:
//			BarDiameter = 1.27;
//			break;
//	}
//	return BarDiameter;
//}
//
//double BarArea(int BarsSize) {        // in^2
//	double BarArea;
//	switch (BarsSize) {
//		case 3:
//			BarArea = 0.11;
//			break;
//		case 4:
//			BarArea = 0.2;
//			break;
//		case 5:
//			BarArea = 0.31;
//			break;
//		case 6:
//			BarArea = 0.44;
//			break;
//		case 7:
//			BarArea = 0.6;
//			break;
//		case 8:
//			BarArea = 0.79;
//			break;
//		case 9:
//			BarArea = 1;
//			break;
//		case 10:
//			BarArea = 1.27;
//			break;
//	}
//	return BarArea;
//}

