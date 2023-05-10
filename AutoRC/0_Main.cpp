#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
using namespace std;

// 複製向量
template<typename T>
vector<T> create_copy(vector<T> const& vec) {
	vector<T> v(vec.size());
	copy(vec.begin(), vec.end(), v.begin());
	return v;
}

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
double TorsionBarDesignQuantity(double, double, double, double, double, double, double);
double TorsionBarSpace(double);


// Other Zone
double BarDiameter(int);
double BarArea(int);
double Fyt(int);

// Global Variable
const int MainBarGrade = 60;
const int StirrupGrade = 60;
const int TorsionBarGrade = 60;
const double dagg = 3.0 / 4.0;
const double Lbin2Kft = 1 / 12 * 1E-3;

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

		// 計算扭矩筋支數
		auto Stirrup_For_Torsion = StirrupForTorsion(Tu, Fyt(TorsionBarGrade), aoh);
		auto Torsion_Bar_Design_Quantity = TorsionBarDesignQuantity(bw, fc, fy, Fyt(TorsionBarGrade), Stirrup_For_Torsion, ph, acp);
		int TorsionBarCount = ceil(Torsion_Bar_Design_Quantity / BarArea(TorsionBar));
		TorsionBarCount += TorsionBarCount % 2;
		int HorizontalTorsionBarCount = ceil((bw - 2.0 * cc - 2.0 * BarDiameter(Stirrup) + TorsionBarSpace(ph)) / (BarDiameter(TorsionBar) + TorsionBarSpace(ph)));
		int VerticalTorsionBarCount = ceil((h - 2.0 * cc - 2.0 * BarDiameter(Stirrup) + TorsionBarSpace(ph)) / (BarDiameter(TorsionBar) + TorsionBarSpace(ph)));
		if (TorsionBarCount > 2 * (HorizontalTorsionBarCount + VerticalTorsionBarCount) - 4) {
			HorizontalTorsionBarCount += ceil((TorsionBarCount - 2 * (HorizontalTorsionBarCount + VerticalTorsionBarCount) + 4) / 2);
			VerticalTorsionBarCount += ceil((TorsionBarCount - 2 * (HorizontalTorsionBarCount + VerticalTorsionBarCount) + 4) / 2);
		}

		// 計算主筋支數
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
		vector <int> Mn1_PerLayerCount;
		vector <int> Mn2_PerLayerCount;
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
		int UpperCount2 = 2;
		int LowerCount2 = 2;
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
			// 將每層筋支數合併為一個向量
			reverse(LowerEachLayerCount.begin(), LowerEachLayerCount.end()); // 將下層配筋順序方向與上層同步
			Mn2_PerLayerCount.insert(Mn2_PerLayerCount.begin(), UpperEachLayerCount.begin(), UpperEachLayerCount.end());
			Mn2_PerLayerCount.insert(Mn2_PerLayerCount.end(), LowerEachLayerCount.begin(), LowerEachLayerCount.end());
			// 計算每層有效深度
			Mn2_EffectiveDepthGuessC.resize(Mn2_PerLayerCount.size());
			Mn2_EffectiveDepthGuessC[0] = cc + BarDiameter(Stirrup) + (BarDiameter(MainBar) / 2);
			Mn2_EffectiveDepthGuessC[(Mn2_EffectiveDepthGuessC.size() - 1)] = h - cc - BarDiameter(Stirrup) - (BarDiameter(MainBar) / 2);
			if (UpperLayers > 1) {
				for (int layer = 1; layer <= UpperLayers - 1; layer++) {
					Mn2_EffectiveDepthGuessC[layer] += (BarDiameter(MainBar) + 1);
				}
			}
			if (LowerLayers > 1) {
				for (int layer = Mn2_EffectiveDepthGuessC.size() - 2; layer > UpperLayers - 1; layer--) {
					Mn2_EffectiveDepthGuessC[layer] -= (BarDiameter(MainBar) + 1);
				}
			}
			// 計算Mn+
			Mn1_EPSILON.resize(Mn2_PerLayerCount.size());
			Mn1_PerLayerCount.resize(Mn2_PerLayerCount.size());
			Mn1_EffectiveDepthGuessC.resize(Mn2_PerLayerCount.size());
			Mn1_EffectiveDepthGuessC = create_copy(Mn2_EffectiveDepthGuessC);
			reverse(Mn1_EffectiveDepthGuessC.begin(), Mn1_EffectiveDepthGuessC.end());
			Mn1_PerLayerCount.resize(Mn2_PerLayerCount.size());
			Mn1_PerLayerCount = create_copy(Mn2_PerLayerCount);
			reverse(Mn1_EffectiveDepthGuessC.begin(), Mn1_EffectiveDepthGuessC.end());
			Mn1 = Moment(beff, h, fc, fy, MainBar, Mn1_EffectiveDepthGuessC, Mn1_PerLayerCount, TorsionBar, HorizontalTorsionBarCount, false, Mn1_EPSILON, Mn1_COMPRESSIVESTRESSZONE);
			// 計算Mn-
			Mn2_EPSILON.resize(Mn2_PerLayerCount.size());
			Mn2 = -Moment(bw, h, fc, fy, MainBar, Mn2_EffectiveDepthGuessC, Mn2_PerLayerCount, TorsionBar, HorizontalTorsionBarCount, false, Mn2_EPSILON, Mn2_COMPRESSIVESTRESSZONE);
			// 計算撓曲折減係數
			if (Si == true) {
				ReductionFactorMn1 = ReductionFactorMforSpiral(fy, Mn1_EPSILON);
				ReductionFactorMn2 = ReductionFactorMforSpiral(fy, Mn2_EPSILON);
			}
			else {
				ReductionFactorMn1 = ReductionFactorMforOther(fy, Mn1_EPSILON);
				ReductionFactorMn2 = ReductionFactorMforOther(fy, Mn2_EPSILON);
			}
			ReduceMn1 = ReductionFactorMn1 * Mn1 / 12000; // * Lbin2Kft;
			ReduceMn2 = ReductionFactorMn2 * Mn2 / 12000; // * Lbin2Kft;
			// 加支數區
			if (ReduceMn1 < Mu1) {
				if (UpperLayers == 1 && UpperCount1 < MaxMainBarCount) {
					UpperCount1 += 1;
				}
				else if (UpperLayers == 1 && UpperCount1 == MaxMainBarCount) {
					UpperLayers += 1;
				}
				else if (UpperLayers > 1 && MaxMainBarCount - UpperCount2 > 1) {
					UpperCount2 += 2;
				}
				else {
					UpperCount2 += 2;
				}
			}
			if (ReduceMn2 > Mu2) {
				if (LowerLayers == 1 && LowerCount1 < MaxMainBarCount) {
					LowerCount1 += 1;
				}
				else if (LowerLayers == 1 && LowerCount1 == MaxMainBarCount) {
					LowerLayers += 1;
				}
				else if (LowerLayers > 1 && MaxMainBarCount - LowerCount2 > 1) {
					UpperCount2 += 2;
				}
				else {
					LowerCount2 += 2;
				}
			}
			cout << ReduceMn1 << " >= " << Mn1 << endl;
			cout << ReduceMn2 << " <= " << Mu2 << endl;
			if (ReduceMn1 >= Mu1 && ReduceMn2 <= Mu2) {
				Reinforcement = false;
			}
		} while (Reinforcement);

		// 計算箍筋及繫筋

		// 檢驗區
		cout << "最小水平支數 = " << MinHorizontalMainBarCount << endl 
			<< "最大水平支數 = " << MaxHorizontalMainBarCount << endl 
			<< "第一層筋支數 = " << Mn2_PerLayerCount[0] << endl 
			<< "最後層筋支數 = " << Mn2_PerLayerCount[Mn2_PerLayerCount.size()-1] << endl 
			<< "Mu+ = " << Mu1 << endl << "Mu- = " << Mu2 << endl << "Mn+ = " << ReduceMn1 << endl << "Mn- = " << ReduceMn2 << endl;
	}
	else {
		// Reinforcement
	}

	system("pause");
	return 0;
}