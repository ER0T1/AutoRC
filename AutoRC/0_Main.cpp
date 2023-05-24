#include <iostream>
#include < fstream >
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
using namespace std;

// Moment Zone
double Beff(double, double, double, double, double, bool);
double AsMinValue(double, double, double, double);
double AsMaxValue(double, double, double);
double Moment(double, double, double, double, int, vector <double>, vector <int>, int, int, bool, vector <double>&, double&);
double ReductionFactorMforOther(double, vector <double>);
double ReductionFactorMforSpiral(double, vector <double>);

// Shear Zone
double Vc(double, double, double);
bool SectionSizeConfirmationForShear(double, double, double, double, double);

// Torsion Zone
double Acp(double, double, double, bool);
double Pcp(double, double, double, bool);
double Ph(double, double, double, double);
double Aoh(double, double, double, int);
double StirrupForTorsion(double, double, double);
bool GeometryConfirmation(double, double, double, double);
bool SectionSizeConfirmationForTorsion(double, double, double, double, double, double, double, double);
double TorsionBarDesignQuantity(double, double, double, double, double, double, double, double, double);
double TorsionBarSpace(double, int, int ,int);
double TorsionBarDesignStrength(double, double, double, double, double, int, int);


// Other Zone
double BarDiameter(int);
double BarArea(int);
double Fyt(int);

// Global Variable
const int MainBarGrade = 60;
const int StirrupGrade = 60;
const int TorsionBarGrade = 60;
const double dagg = 3.0 / 4.0;
const double Lbin2Kft = 1. / 12. * 1E-3;

int main() {
	// Read Strength Information:
	double Mu1 = 158.507, Mu2 = -308.283, Vu1 = 67.4, Vu2 = -71.41, Tu = 58.153; // klb-ft, klb, klb-ft
	// Read Size Information:
	double Sw = 260.0, bw = 21.0, hf = 12.0, h = 28.0, bf = 84, ln = 236.0; // in
	// Read Material Information:
	double fc = 5000.0, fy = 60000.0; // psi

	// bw, h Confirmation
	if (bw < 0.3 * h || bw < 10) {
		cout << "Your beam has insufficient section dimensions, please increase the beam width!" << endl;
		system("pause");
		exit(0);
	}
	bool SkinRebar = false;
	if (h >= 36) {
		SkinRebar = true;
	}

	// Beam Type Confirmation
	cout << "Whether it is the Middle Beam or the Side Beam? (S/M)" << endl;
	bool WingWidthType;
	string WidthType;
	while (true) {
		cin >> WidthType;
		if (WidthType == "S" || WidthType == "s" || WidthType == "Side" || WidthType == "side" || WidthType == "邊梁" || WidthType == "邊") {
			cout << "It is Side Beam." << endl;
			WingWidthType = true;
			break;
		}
		else if (WidthType == "M" || WidthType == "m" || WidthType == "Middle" || WidthType == "middle" || WidthType == "中間梁" || WidthType == "中間" || WidthType == "中") {
			cout << "It is Middle Beam." << endl;
			WingWidthType = false;
			break;
		}
		else {
			cout << "Please answer correctly!\nWhether it is the Middle Beam or the Side Beam? (S/M)" << endl;
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

	//Enter the Rebar number
	int MainBar;
	cout << "Please enter the Longitudinal Reinforcement Number." << endl;
	cin >> MainBar;
	cout << "You use No." << MainBar << " rebar as the Longitudinal Reinforcement." << endl;

	int Stirrup;
	cout << "Please enter the Stirrup Number." << endl;
	cin >> Stirrup;
	cout << "You use No." << Stirrup << " rebar as the Stirrup." << endl;

	int TorsionBar;
	cout << "Please enter the Torsion Bars Number." << endl;
	cin >> TorsionBar;
	cout << "You use No." << TorsionBar << " rebar as the Torsion Bars." << endl;

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

	// Basic Parameter Calculation
	auto beff = Beff(bw, Sw, hf, ln, bf, WingWidthType);
	auto EffectiveDepth = 0.9 * h;  // 0.9 is the estimated value of effective depth
	auto acp = Acp(bw, h, hf, WingWidthType);
	auto pcp = Pcp(bw, h, hf, WingWidthType);
	auto ph = Ph(bw, h, cc, Stirrup);
	auto aoh = Aoh(bw, h, cc, Stirrup);
	auto ao = 0.85 * Aoh(bw, h, cc, Stirrup);
	auto vc = Vc(bw, fc, EffectiveDepth);
	auto Vu = max({ abs(Vu1),abs(Vu2) });

	// Geometry Confirmation ( Confirm whether torsion bars are required. )
	if (GeometryConfirmation(Tu, fc, acp, pcp)) {
		//Section Size Confirmation
		if (!SectionSizeConfirmationForTorsion(bw, Vu, Tu, fc, EffectiveDepth, ph, aoh, vc)) {
			cout << "Your section size for Torsion is insufficient!\nPlease reset the section." << endl;
			system("pause");
			exit(0);
		}
		if (!SectionSizeConfirmationForShear(bw, Vu, fc, vc, EffectiveDepth)) {
			cout << "Your section size for Shear is insufficient!\nPlease reset the section." << endl;
			system("pause");
			exit(0);
		}

		// Reinforcement

		// 計算扭矩筋
		auto Stirrup_For_Torsion = StirrupForTorsion(Tu, TorsionBarGrade, aoh);
		auto Torsion_Bar_Design_Quantity = TorsionBarDesignQuantity(Tu, bw, fc, fy, TorsionBarGrade, Stirrup_For_Torsion, ph, acp, aoh);
		int TorsionBarCount = ceil(Torsion_Bar_Design_Quantity / BarArea(TorsionBar));
		TorsionBarCount += TorsionBarCount % 2;
		int HorizontalTorsionBarCount = ceil((bw - 2.0 * cc - 2.0 * BarDiameter(Stirrup) + TorsionBarSpace(ph, MainBar, Stirrup, TorsionBar)) / (BarDiameter(TorsionBar) + TorsionBarSpace(ph, MainBar, Stirrup, TorsionBar)));
		int VerticalTorsionBarCount = ceil((h - 2.0 * cc - 2.0 * BarDiameter(Stirrup) + TorsionBarSpace(ph, MainBar, Stirrup, TorsionBar)) / (BarDiameter(TorsionBar) + TorsionBarSpace(ph, MainBar, Stirrup, TorsionBar)));
		if (TorsionBarCount > 2 * (HorizontalTorsionBarCount + VerticalTorsionBarCount) - 4) {
			HorizontalTorsionBarCount += ceil((TorsionBarCount - 2 * (HorizontalTorsionBarCount + VerticalTorsionBarCount) - 4) / 2);
			VerticalTorsionBarCount += ceil((TorsionBarCount - 2 * (HorizontalTorsionBarCount + VerticalTorsionBarCount) - 4) / 2);
		}
		TorsionBarCount = 2 * (HorizontalTorsionBarCount + VerticalTorsionBarCount) - 4;
		auto Tn = TorsionBarDesignStrength(fy, aoh, Stirrup_For_Torsion, TorsionBarGrade, ph, TorsionBarCount, TorsionBar);
		auto ReduceTn = 0.75 * Tn;

		// 除錯用
		//cout << Tn << endl;
		//cout << ReduceTn << endl;

		// 計算主筋
		auto MinMainBarSpace = min({ 4.0 / 3.0 * dagg, BarDiameter(MainBar), 1.0 });
		auto MaxMainBarSpace = min({ 900000 / fy - 2.5 * cc,720000 / fy });
		auto MinMainBarQuantity = AsMinValue(bw, fc, fy, EffectiveDepth);
		auto MaxMainBarQuantity = AsMaxValue(bw, EffectiveDepth, Fyt(MainBarGrade));
		int MinHorizontalMainBarCount = ceil((bw - 2.0 * cc - 2.0 * BarDiameter(Stirrup) + MaxMainBarSpace) / (BarDiameter(MainBar) + MaxMainBarSpace));
		int MaxHorizontalMainBarCount = floor((bw - 2.0 * cc - 2.0 * BarDiameter(Stirrup) + MinMainBarSpace) / (BarDiameter(MainBar) + MinMainBarSpace));
		int MinMainBarCount = ceil(MinMainBarQuantity / BarArea(MainBar));
		int MaxMainBarCount = floor(MaxMainBarQuantity / BarArea(MainBar));
		if (MinHorizontalMainBarCount < MinMainBarCount) {
			MinHorizontalMainBarCount = MinMainBarCount;
		}

		// 排列及計算
		int UpperLayers = 1, LowerLayers = 1;
		vector <int> UpperEachLayerCount(UpperLayers);
		vector <int> LowerEachLayerCount(LowerLayers);
		int AllUpperCount;
		int AllLowerCount;
		vector <int> Mn1_PerLayerCount;
		vector <int> Mn2_PerLayerCount;
		vector <double> UpperLayersDepthGuessC;
		vector <double> LowerLayersDepthGuessC;
		vector <double> Mn1_EffectiveDepthGuessC;
		vector <double> Mn2_EffectiveDepthGuessC;
		vector <double> Mn1_EPSILON;
		vector <double> Mn2_EPSILON;
		double Mn1_COMPRESSIVESTRESSZONE;
		double Mn2_COMPRESSIVESTRESSZONE;
		double Mn1;
		double Mn2;
		double ReductionFactorMn1;
		double ReductionFactorMn2;
		double ReduceMn1;
		double ReduceMn2;
		int UpperCount1 = MinHorizontalMainBarCount;
		int LowerCount1 = MinHorizontalMainBarCount;
		int UpperCount2 = 0;
		int LowerCount2 = 0;
		bool Reinforcement = true;
		do {
			// 上層筋
			if (UpperLayers == 1) {
				UpperEachLayerCount[0] = UpperCount1;
			}
			else {
				for (int UpperLayer = 0; UpperLayer < UpperLayers - 1; UpperLayer++) {
					UpperEachLayerCount.resize(UpperLayers);
					UpperEachLayerCount[UpperLayer] = MaxHorizontalMainBarCount;
				}
				UpperEachLayerCount[UpperLayers - 1] = UpperCount2;
			}
			// 檢核上層筋鋼筋量
			AllUpperCount = 0;
			for (int Layer = 0; Layer < UpperLayers; Layer++) {
				AllUpperCount += UpperEachLayerCount[Layer];
			}
			if (AllUpperCount > MaxMainBarCount) {
				cout << "Your section size for Amount of Reinforcement is insufficient!\nPlease reset the section." << endl;
				system("pause");
				exit(0);
			}

			// 除錯用
			//cout << AllUpperCount << endl;

			// 下層筋
			if (LowerLayers == 1) {
				LowerEachLayerCount[0] = LowerCount1;
			}
			else {
				for (int LowerLayer = 0; LowerLayer < LowerLayers - 1; LowerLayer++) {
					LowerEachLayerCount.resize(LowerLayers);
					LowerEachLayerCount[LowerLayer] = MaxHorizontalMainBarCount;
				}
				LowerEachLayerCount[LowerLayers - 1] = LowerCount2;
			}
			// 檢核下層筋鋼筋量
			AllLowerCount = 0;
			for (int Layer = 0; Layer < LowerLayers; Layer++) {
				AllLowerCount += LowerEachLayerCount[Layer];
			}
			if (AllLowerCount > MaxMainBarCount) {
				cout << "Your section size for Amount of Reinforcement is insufficient!\nPlease reset the section." << endl;
				system("pause");
				exit(0);
			}

			// 除錯用
			//cout << AllLowerCount << endl;

			// 將每層筋支數合併為一個向量
			Mn2_PerLayerCount = UpperEachLayerCount;
			Mn2_PerLayerCount.insert(Mn2_PerLayerCount.end(), LowerEachLayerCount.rbegin(), LowerEachLayerCount.rend());
			// 計算每層有效深度
			UpperLayersDepthGuessC.resize(UpperLayers);
			LowerLayersDepthGuessC.resize(LowerLayers);
			UpperLayersDepthGuessC[0] = cc + BarDiameter(Stirrup) + (BarDiameter(MainBar) / 2);
			LowerLayersDepthGuessC[0] = h - cc - BarDiameter(Stirrup) - (BarDiameter(MainBar) / 2);
			for (int layer = 1; layer <= UpperLayers - 1; layer++) {
				UpperLayersDepthGuessC[layer] = UpperLayersDepthGuessC[layer - 1] + BarDiameter(MainBar) + 1;
			}
			for (int layer = 1; layer <= LowerLayers - 1; layer++) {
				LowerLayersDepthGuessC[layer] = LowerLayersDepthGuessC[layer - 1] - BarDiameter(MainBar) - 1;
			}
			// 合併上下層有效深度
			Mn2_EffectiveDepthGuessC = UpperLayersDepthGuessC;
			Mn2_EffectiveDepthGuessC.insert(Mn2_EffectiveDepthGuessC.end(), LowerLayersDepthGuessC.rbegin(), LowerLayersDepthGuessC.rend());

			// 除錯用
			//for (int i = 0; i < Mn2_PerLayerCount.size(); i++) {
			//	cout << Mn2_PerLayerCount[i] << ", ";
			//}
			//cout << endl;
			//for (int i = 0; i < Mn2_EffectiveDepthGuessC.size(); i++) {
			//	cout << Mn2_EffectiveDepthGuessC[i] << ", ";
			//}
			//cout << endl;

			// Mn+ 參數
			Mn1_EPSILON.resize(Mn2_PerLayerCount.size());
			Mn1_PerLayerCount.resize(Mn2_PerLayerCount.size());
			Mn1_EffectiveDepthGuessC = Mn2_EffectiveDepthGuessC;
			reverse(Mn1_EffectiveDepthGuessC.begin(), Mn1_EffectiveDepthGuessC.end());
			for (int i = 0; i < Mn1_EffectiveDepthGuessC.size(); i++) {
				Mn1_EffectiveDepthGuessC[i] = h - Mn1_EffectiveDepthGuessC[i];
			}
			Mn1_PerLayerCount.resize(Mn2_PerLayerCount.size());
			Mn1_PerLayerCount = Mn2_PerLayerCount;
			reverse(Mn1_PerLayerCount.begin(), Mn1_PerLayerCount.end());
			// Mn- 參數
			Mn2_EPSILON.resize(Mn2_PerLayerCount.size());
			// 計算Mn+
			Mn1 = Moment(bw, h, fc, fy, MainBar, Mn1_EffectiveDepthGuessC, Mn1_PerLayerCount, TorsionBar, HorizontalTorsionBarCount, false, Mn1_EPSILON, Mn1_COMPRESSIVESTRESSZONE);
			// 計算Mn-
			Mn2 = -Moment(beff, h, fc, fy, MainBar, Mn2_EffectiveDepthGuessC, Mn2_PerLayerCount, TorsionBar, HorizontalTorsionBarCount, false, Mn2_EPSILON, Mn2_COMPRESSIVESTRESSZONE);
			// 計算撓曲折減係數
			if (Si == true) {
				ReductionFactorMn1 = ReductionFactorMforSpiral(fy, Mn1_EPSILON);
				ReductionFactorMn2 = ReductionFactorMforSpiral(fy, Mn2_EPSILON);
			}
			else {
				ReductionFactorMn1 = ReductionFactorMforOther(fy, Mn1_EPSILON);
				ReductionFactorMn2 = ReductionFactorMforOther(fy, Mn2_EPSILON);
			}
			ReduceMn1 = ReductionFactorMn1 * Mn1 * Lbin2Kft;
			ReduceMn2 = ReductionFactorMn2 * Mn2 * Lbin2Kft;

			// 除錯用
			//cout << "Mu+ = " << Mu1 << endl << "Mu- = " << Mu2 << endl << "φMn+ = " << ReduceMn1 << endl << "φMn- = " << ReduceMn2 << endl;
			//for (int i = 0; i < Mn2_PerLayerCount.size(); i++) {
			//	cout << Mn2_PerLayerCount[i] << ", ";
			//}
			//cout << endl;
			//for (int i = 0; i < Mn2_EffectiveDepthGuessC.size(); i++) {
			//	cout << Mn2_EffectiveDepthGuessC[i] << ", ";
			//}
			//cout << endl;

			// 跳出迴圈
			if (ReduceMn1 >= Mu1 && ReduceMn2 <= Mu2) {
				break;
			}
			// 加支數區
			if (ReduceMn1 < Mu1) {
				if (UpperLayers == 1 && UpperCount1 < MaxHorizontalMainBarCount) {
					UpperCount1 += 1;
				}
				else if (UpperLayers == 1 && UpperCount1 == MaxHorizontalMainBarCount) {
					UpperLayers += 1; UpperCount2 = 2;
				}
				else if (UpperLayers > 1 && MaxHorizontalMainBarCount - UpperCount2 > 1) {
					UpperCount2 += 2;
				}
				else if (UpperLayers > 1 && MaxHorizontalMainBarCount - UpperCount2 == 1) {
					UpperCount2 += 1;
				}
				else if (UpperLayers > 1 && MaxHorizontalMainBarCount == UpperCount2) {
					UpperLayers += 1; UpperCount2 = 2;
				}
			}
			if (ReduceMn2 > Mu2) {
				if (LowerLayers == 1 && LowerCount1 < MaxHorizontalMainBarCount) {
					LowerCount1 += 1;
				}
				else if (LowerLayers == 1 && LowerCount1 == MaxHorizontalMainBarCount) {
					LowerLayers += 1; LowerCount2 = 2;
				}
				else if (LowerLayers > 1 && MaxHorizontalMainBarCount - LowerCount2 > 1) {
					LowerCount2 += 2;
				}
				else if (LowerLayers > 1 && MaxHorizontalMainBarCount - LowerCount2 == 1) {
					LowerCount2 += 1;
				}
				else if (LowerLayers > 1 && MaxHorizontalMainBarCount == LowerCount2) {
					LowerLayers += 1; LowerCount2 = 2;
				}
			}
		} while (Reinforcement);

		// 計算箍筋及繫筋



		// 檢驗區
		cout << "撓曲筋最小水平支數 = " << MinHorizontalMainBarCount << endl
			<< "撓曲筋最大水平支數 = " << MaxHorizontalMainBarCount << endl
			<< "撓曲筋最小總支數 = " << MinMainBarCount << endl
			<< "撓曲筋最大總支數 = " << MaxMainBarCount << endl
			<< "  Mu+ = " << Mu1 << endl << "  Mu- = " << Mu2 << endl << "φMn+ = " << ReduceMn1 << endl << "φMn- = " << ReduceMn2 << endl;
		cout << "每層支數 = { ";
		for (int i = 0; i < Mn1_PerLayerCount.size(); i++) {
			cout << Mn1_PerLayerCount[i] << ", ";
		}
		cout << " }" << endl;
		cout << "每層深度 = { ";
		for (int i = 0; i < Mn1_EffectiveDepthGuessC.size(); i++) {
			cout << Mn1_EffectiveDepthGuessC[i] << ", ";
		}
		cout << " }" << endl;
		//cout << "Mn1_C = " << Mn1_COMPRESSIVESTRESSZONE << endl;
		//cout << "Mn2_C = " << Mn2_COMPRESSIVESTRESSZONE << endl;
		cout << "扭矩筋水平支數 = " << HorizontalTorsionBarCount << endl
			<< "扭矩筋垂直支數 = " << VerticalTorsionBarCount << endl
			<< "扭矩筋總支數 = " << 2 * (HorizontalTorsionBarCount + VerticalTorsionBarCount) - 4 << endl;
		cout <<  "  Tu = " << Tu << endl << "φTn = " << ReduceTn << endl;
	}
	else {
		// Reinforcement
	}

	system("pause");
	return 0;
}