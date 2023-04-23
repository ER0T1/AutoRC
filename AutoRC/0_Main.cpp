#include <iostream>
#include < fstream >
#include <vector>
#include <cmath>
using namespace std;

// Moment Zone
double AsMinValue(double, double, double, double);
double AsMaxValue(double, double, double);
double Beff(double, double, double, double, bool);
double Moment(double, double, double, double, double, double, double, double, int, int, int, int, int, int, bool, vector <double>&, double&);
double ReductionFactorMforOther(int, double, vector <double>&);
double ReductionFactorMforSpiral(int, double, vector <double>&);

// Shear Zone

// Torsion Zone
bool GeometryConfirmation(double, double, double, double, double, bool);
double StirrupForTorsion(double, double, double, double, double, double);
double TorsionBarDesignQuantity(double, double, double, double, double, double, double, double, double, bool);
double TorsionBarSpace(double, double, double, double);

// Other Zone
double BarDiameter(int);
double BarArea(int);
double Fyt(int);

// Global Variable
const int MainReinGrade = 60;
const int StirrupGrade = 60;
const int TorsionBarGrade = 60;
const double dagg = 3.0 / 4.0;
const double Lbin2Kft = 1 / 12000;

int main() {
	// Read Strength Information:
	double Mu1 = 98, Mu2 = -102, Vu = 12, Tu = 37; // klb-ft, klb, klb-ft
	// Read Size Information:
	double Sw = 200.0, bw = 15.0, hf = 5.0, h = 20.0, ln = 200.0; // in
	// Read Material Information:
	double fc = 5000.0, fy = 60000.0; // psi

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
	string WidthType;
	while (true) {
		cin >> WidthType;
		if (WidthType == "S" || WidthType == "s" || WidthType == "Single" || WidthType == "single" || WidthType == "單邊" || WidthType == "單") {
			cout << "It is Single Wing." << endl;
			WingWidthType = true;
			break;
		}
		else if (WidthType == "D" || WidthType == "d" || WidthType == "Double" || WidthType == "double" || WidthType == "雙邊" || WidthType == "雙") {
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
	string EarthquakeArea;
	cout << "Is it designed for strong earthquake areas? (Y/N)" << endl;
	while (true) {
		cin >> EarthquakeArea;
		if (EarthquakeArea == "Y" || EarthquakeArea == "y" || EarthquakeArea == "Yes" || EarthquakeArea == "yes" || EarthquakeArea == "是") {
			Ei = true;
			cout << "It is designed for strong earthquake areas." << endl;
			break;
		}
		else if (EarthquakeArea == "N" || EarthquakeArea == "n" || EarthquakeArea == "No" || EarthquakeArea == "no" || EarthquakeArea == "否") {
			Ei = false;
			cout << "It is'nt designed for strong earthquake areas." << endl;
			break;
		}
		else {
			cout << "Please answer correctly!\nIs it designed for strong earthquake areas? (Y/N)" << endl;
		}
	}

	// Design Clear Cover thickness
	double cc;
	cout << "Please enter the Clear Cover thickness in inch." << endl;
	bool ClearCoverDesign = true;
	while (ClearCoverDesign) {
		cin >> cc;
		if (cc > 4) {
			cout << "Please enter the correct value!\nThe thickness of Clear Cover must not exceed 4 inches!\nIt is recommended to set the cover thickness between 1.5 and 3 inches.\nPlease enter the Clear Cover thickness in inch." << endl;
		}
		else if (cc > 3) {
			cout << "It is recommended to set the cover thickness between 1.5 and 3 inches.\nWould you like to change the design? (Y/N)" << endl;
			string ChangeClearCover;
			while (bool ChangeClearCoverF = true) {
				cin >> ChangeClearCover;
				if (ChangeClearCover == "Y" || ChangeClearCover == "y" || ChangeClearCover == "Yes" || ChangeClearCover == "yes" || ChangeClearCover == "是") {
					cout << "Please enter the Clear Cover thickness in inch." << endl;
					ChangeClearCoverF = false;
				}
				else if (ChangeClearCover == "N" || ChangeClearCover == "n" || ChangeClearCover == "No" || ChangeClearCover == "no" || ChangeClearCover == "否") {
					cout << "Clear Cover thickness is " << cc << " inches." << endl;
					ChangeClearCoverF = false;
					ClearCoverDesign = false;
				}
				else {
					cout << "Please answer correctly!\nWould you like to change the design? (Y/N)" << endl;
				}
			}
		}
		else if (cc < 1.5) {
			cout << "Please enter the correct value!\nThe thickness of Clear Cover must be no less than 1.5 inches!\nIt is recommended to set the cover thickness between 1.5 and 3 inches.\nPlease enter the Clear Cover thickness in inch." << endl;
		}
		else {
			cout << "Clear Cover thickness is " << cc << " inches." << endl;
			ClearCoverDesign = false;
		}
	}

	//Stirrup Information
	bool Si;
	string StirrupInformation;
	cout << "Whether it is designed for Spiral Stirrup? (Y/N)" << endl;
	while (true) {
		cin >> StirrupInformation;
		if (StirrupInformation == "Y" || StirrupInformation == "y" || StirrupInformation == "Yes" || StirrupInformation == "yes" || StirrupInformation == "是") {
			Si = true;
			cout << "It is designed for Spiral Stirrup." << endl;
			break;
		}
		else if (StirrupInformation == "N" || StirrupInformation == "n" || StirrupInformation == "No" || StirrupInformation == "no" || StirrupInformation == "否") {
			Si = false;
			cout << "It is'nt designed for Spiral Stirrup." << endl;
			break;
		}
		else {
			cout << "Please answer correctly!\nWhether it is designed for Spiral Stirrup? (Y/N)" << endl;
		}
	}

	// Geometry Confirmation ( Confirm whether torsion bars are required. )
	if (GeometryConfirmation(Tu, bw, hf, h, fc, WingWidthType)) {
		// Reinforcement ( Confirm whether there is a strong earthquake area. )
		// 固定鋼筋保護層1.5英吋，箍筋從#4開始，扭矩筋從#3開始，主筋從#6開始
		for (int MainRein = 5; MainRein <= 10; MainRein++) {
			for (int Stirrup = 3; (Stirrup < MainRein && Stirrup < 6); Stirrup++) {
				for (int TorsionBar = 3; (TorsionBar <= MainRein && TorsionBar <= 7); TorsionBar++) {

					// 計算扭矩筋支數
					auto Torsion_Bar_Design_Quantity = TorsionBarDesignQuantity(Tu, bw, h, hf, fc, fy, Fyt(TorsionBarGrade), cc, Stirrup, WingWidthType);
					int TorsionBarCount = ceil(Torsion_Bar_Design_Quantity / BarArea(TorsionBar));
					TorsionBarCount += TorsionBarCount % 2;
					int HorizontalTorsionBarCount = ceil((bw - 2.0 * cc - 2.0 * BarDiameter(Stirrup) + TorsionBarSpace(bw, h, cc, Stirrup)) / (BarDiameter(TorsionBar) + TorsionBarSpace(bw, h, cc, Stirrup)));
					int VerticalTorsionBarCount = ceil((h - 2.0 * cc - 2.0 * BarDiameter(Stirrup) + TorsionBarSpace(bw, h, cc, Stirrup)) / (BarDiameter(TorsionBar) + TorsionBarSpace(bw, h, cc, Stirrup)));
					while (true) {
						if (TorsionBarCount > 2 * (HorizontalTorsionBarCount + VerticalTorsionBarCount) - 4) {
							HorizontalTorsionBarCount += 1;
							VerticalTorsionBarCount += 1;
						}
						else {
							break;
						}
					}
					/*if (3 * Torsion_Bar_Design_Quantity < (2 * (HorizontalTorsionBarCount + VerticalTorsionBarCount) - 4) * BarArea(TorsionBar)) {
						break;
					}*/
					//cout << "Torsion Bar Count = " << TorsionBarCount << " Horizontal Torsion Bar Count = " << HorizontalTorsionBarCount << " Vertical Torsion Bar Count = " << VerticalTorsionBarCount << endl;

					// 計算主筋支數
					auto EffectiveDepth = h - cc - BarDiameter(Stirrup) - BarDiameter(MainRein) / 2.0;
					auto MainReinSpace = min({ 4.0 / 3.0 * dagg, BarDiameter(MainRein), 1.0 });
					int HorizontalMainReinCount = floor((bw - 2.0 * cc - 2.0 * BarDiameter(Stirrup) + MainReinSpace) / (BarDiameter(MainRein) + MainReinSpace));
					int MainReinCountLowerLimit = ceil(AsMinValue(bw, fc, fy, EffectiveDepth) / BarArea(MainRein));
					int MainReinCountUperLimit = floor(AsMaxValue(bw, EffectiveDepth, Fyt(MainReinGrade)) / BarArea(MainRein));
					MainReinCountUperLimit = min({ HorizontalMainReinCount, MainReinCountUperLimit });

					// 配筋
					for (int MainReinLayers = 2; MainReinLayers <= 6; MainReinLayers += 1) {
						for (int MainReinCount = MainReinCountLowerLimit; MainReinCount <= MainReinCountUperLimit; MainReinCount++) {
							for (int count = 0; count <= MainReinCount; count += 2) {

								vector <double> EPSILON(6);
								double COMPRESSIVESTRESSZONE;
								auto beff = Beff(bw, Sw, hf, ln, WingWidthType);
								auto Mn1 = Moment(beff, h, hf, ln, fc, fy, cc, Stirrup, MainRein, MainReinCount, TorsionBar, HorizontalTorsionBarCount, MainReinLayers, count, false, EPSILON, COMPRESSIVESTRESSZONE);
								auto Mn2 = -Moment(bw, h, hf, ln, fc, fy, cc, Stirrup, MainRein, MainReinCount, TorsionBar, HorizontalTorsionBarCount, MainReinLayers, count, false, EPSILON, COMPRESSIVESTRESSZONE);

								double ReductionFactorM;
								if (Si == true) {
									ReductionFactorM = ReductionFactorMforSpiral(MainReinLayers, fy, EPSILON);
								}
								else {
									ReductionFactorM = ReductionFactorMforOther(MainReinLayers, fy, EPSILON);
								}

								auto ReduceMn1 = ReductionFactorM * Mn1 * Lbin2Kft;
								auto ReduceMn2 = ReductionFactorM * Mn2 * Lbin2Kft;

								/*auto Mpr1 = Moment(bw, h, hf, ln, fc, fy, cc, Stirrup, MainRein, MainReinCount, TorsionBar, HorizontalTorsionBarCount, MainReinLayers, Ei);
								auto Mpr2 = Moment(beff, h, hf, ln, fc, fy, cc, Stirrup, MainRein, MainReinCount, TorsionBar, HorizontalTorsionBarCount, MainReinLayers, Ei);
								auto Ve = (Mpr1 + Mpr2) / 2;*/


								if (ReduceMn1 < Mu1 || ReduceMn2 > Mu2) {
									continue;
								}


								// Is the section large enough?
								// Stirrup for shear
								// Stirrup for torsion
								// Design stirrup for shear & torsion
								// Check spacing
								// Design for longtitudinal reinforcement
								cout << "------------------------------------------------------------------------------------------------------------------------" << endl;
								cout << "Clear Cover = " << cc << endl;
								cout << "Main Rein = #" << MainRein << ", Main Rein Count = " << MainReinCount * MainReinLayers << ", The Count of Each Layer = " << MainReinCount << ", Count of Layers = " << MainReinLayers << endl;
								cout << "Stirrup = #" << Stirrup << endl;
								cout << "TorsionBar = #" << TorsionBar << ", Torsion Bar Count = " << 2 * (HorizontalTorsionBarCount + VerticalTorsionBarCount - 2) << ", Horizontal Torsion Bar Count = " << HorizontalTorsionBarCount << ", Vertical Torsion Bar Count = " << VerticalTorsionBarCount << endl;
								cout << "Mu+ = " << Mu1 << " k-ft, Mu- = " << Mu2 << " k-ft, Vu = " << Vu << ", Tu = " << Tu << endl;
								cout << "φMn+ = " << ReduceMn1 << " k-ft, φMn- = " << ReduceMn2 << " k-ft, φVn = " << 0.7 * Vu << ", φTn = " << 0.7 * Tu << endl << endl;
								/*cout << "φMn+ = " << Mn1 / 12000 << " k-ft, φMn- = " << Mn2 / 12000 << " k-ft, φVn = " << 0.7 * Vu << ", φTn = " << 0.7 * Tu << endl << endl;*/

							}
						}
					}
				}
			}
		}
	}
	else {
		// Reinforcement ( Confirm whether there is a strong earthquake area. )
	}

	return 0;
}