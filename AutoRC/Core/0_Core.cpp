#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
using namespace std;

// 輸入用結構
struct InPutData {
	// Read Strength Information:// kips-ft, kips, kips-ft
	double Mu1{}; double Mu2{}; double Vu1{}; double Vu2{}; double Tu{};
	// Read Size Information:// in
	double Sw{}; double bw{}; double hf{}; double h{}; double bf{}; double ln{};
	// Read Material Information:// psi
	double fc{}; double fy{};
	// Coefficient Load Combination:// psi/ft^3
	double Dl{}; double Ll{}; double LoadFactorDL{}; double LoadFactorLL{};
	// Beam Type Confirmation:
	bool WingWidthType{}; string WidthType{};
	// Strong Earthquake Area Confirmation:
	bool Ei{}; string EarthquakeArea{};
	// Enter the Rebar number:
	int MainBar{}; int Stirrup{}; int TorsionBar{};
	// Design Clear Cover thickness:
	double cc{};
	// Stirrup Information:
	bool Si{}; string StirrupInformation{};
	// Global Variable
	double MainBarGrade{}; double StirrupGrade{}; double TorsionBarGrade{}; double dagg{};
	// Unit Setting
	string ForceMultLength{}; string Force{}; string Length{};
};

struct OutPutData {
	// Write Strength Information:// kips-ft, kips, kips-ft
	double Mu1{}; double Mu2{}; double Vu1{}; double Vu2{}; double Tu{};
	// Write Size Information:// in
	double Sw{}; double bw{}; double hf{}; double h{}; double bf{}; double ln{};
	// Write Material Information:// psi
	double fc{}; double fy{};
	// Coefficient Load Combination:// psi/ft^3
	double Dl{}; double Ll{}; double LoadFactorDL{}; double LoadFactorLL{};
	// Beam Type Confirmation:
	bool WingWidthType{}; string WidthType{};
	// Strong Earthquake Area Confirmation:
	bool Ei{}; string EarthquakeArea{};
	// Enter the Rebar number:
	int MainBar{}; int Stirrup{}; int TorsionBar{};
	// Design Clear Cover thickness:
	double cc{};
	// Stirrup Information:
	bool Si{}; string StirrupInformation{};
	// Global Variable
	double MainBarGrade{}; double StirrupGrade{}; double TorsionBarGrade{}; double dagg{};
	// Unit Setting
	string ForceMultLength{}; string Force{}; string Length{};
	// Output Moment Data
	double ReduceMn1{}; double ReduceMn2{}; double ReductionFactorMn1{}; double ReductionFactorMn2{}; int MinHorizontalMainBarCount{}; int MaxHorizontalMainBarCount{}; int MinMainBarCount{}; int MaxMainBarCount{}; vector <int> Mn1_PerLayerCount{}; vector <int> Mn2_PerLayerCount{}; vector <double> Mn1_EffectiveDepthGuessC{}; vector <double> Mn2_EffectiveDepthGuessC{};
	// Output Torsion Data
	double ReduceTn{}; double ReduceT{}; int HorizontalTorsionBarCount{}; int VerticalTorsionBarCount{}; int TotalTorsionBarCount{};
}DataOUT;

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
double Ve(double, double, double, double, double, double, double);

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
double CalculateDL(double, double, double, double, double);
double CalculateLL(double, double);

// Global Variable
// Unit Conversion
const double Lbin2Kft = 1. / 12. * 1E-3;

OutPutData Core(InPutData& DataIN) {
	// bw, h Confirmation
	if (DataIN.bw < 0.3 * DataIN.h || DataIN.bw < 10) {
		cout << "Your beam has insufficient section dimensions, please increase the beam width!" << endl;
	}
	bool SkinRebar = false;
	if (DataIN.h >= 36) {
		SkinRebar = true;
	}

	// Basic Parameter Calculation
	auto beff = Beff(DataIN.bw, DataIN.Sw, DataIN.hf, DataIN.ln, DataIN.bf, DataIN.WingWidthType);
	auto EffectiveDepth = 0.9 * DataIN.h;  // 0.9 is the estimated value of effective depth
	auto acp = Acp(DataIN.bw, DataIN.h, DataIN.hf, DataIN.WingWidthType);
	auto pcp = Pcp(DataIN.bw, DataIN.h, DataIN.hf, DataIN.WingWidthType);
	auto ph = Ph(DataIN.bw, DataIN.h, DataIN.cc, DataIN.Stirrup);
	auto aoh = Aoh(DataIN.bw, DataIN.h, DataIN.cc, DataIN.Stirrup);
	auto ao = 0.85 * Aoh(DataIN.bw, DataIN.h, DataIN.cc, DataIN.Stirrup);
	auto vc = Vc(DataIN.bw, DataIN.fc, EffectiveDepth);
	auto Vu = max({ abs(DataIN.Vu1),abs(DataIN.Vu2) });
	auto Dl = CalculateDL(DataIN.Sw, DataIN.bw, DataIN.h, DataIN.hf, DataIN.Dl);
	auto Ll = CalculateLL(DataIN.Sw, DataIN.Ll);

	// Geometry Confirmation ( Confirm whether torsion bars are required. )
	if (GeometryConfirmation(DataIN.Tu, DataIN.fc, acp, pcp)) {
		auto Ti = true;
		//Section Size Confirmation
		if (!SectionSizeConfirmationForTorsion(DataIN.bw, Vu, DataIN.Tu, DataIN.fc, EffectiveDepth, ph, aoh, vc)) {
			cout << "Your section size for Torsion is insufficient!\nPlease reset the section." << endl;
		}
		if (!SectionSizeConfirmationForShear(DataIN.bw, Vu, DataIN.fc, vc, EffectiveDepth)) {
			cout << "Your section size for Shear is insufficient!\nPlease reset the section." << endl;
		}

		// Reinforcement

		// 計算扭矩筋
		auto Stirrup_For_Torsion = StirrupForTorsion(DataIN.Tu, DataIN.TorsionBarGrade, aoh);
		auto Torsion_Bar_Design_Quantity = TorsionBarDesignQuantity(DataIN.Tu, DataIN.bw, DataIN.fc, DataIN.fy, DataIN.TorsionBarGrade, Stirrup_For_Torsion, ph, acp, aoh);
		int TorsionBarCount = ceil(Torsion_Bar_Design_Quantity / BarArea(DataIN.TorsionBar));
		TorsionBarCount += TorsionBarCount % 2;
		int HorizontalTorsionBarCount = ceil((DataIN.bw - 2.0 * DataIN.cc - 2.0 * BarDiameter(DataIN.Stirrup) + TorsionBarSpace(ph, DataIN.MainBar, DataIN.Stirrup, DataIN.TorsionBar)) / (BarDiameter(DataIN.TorsionBar) + TorsionBarSpace(ph, DataIN.MainBar, DataIN.Stirrup, DataIN.TorsionBar)));
		int VerticalTorsionBarCount = ceil((DataIN.h - 2.0 * DataIN.cc - 2.0 * BarDiameter(DataIN.Stirrup) + TorsionBarSpace(ph, DataIN.MainBar, DataIN.Stirrup, DataIN.TorsionBar)) / (BarDiameter(DataIN.TorsionBar) + TorsionBarSpace(ph, DataIN.MainBar, DataIN.Stirrup, DataIN.TorsionBar)));
		if (TorsionBarCount > 2 * (HorizontalTorsionBarCount + VerticalTorsionBarCount) - 4) {
			HorizontalTorsionBarCount += ceil((TorsionBarCount - 2 * (HorizontalTorsionBarCount + VerticalTorsionBarCount) - 4) / 2);
			VerticalTorsionBarCount += ceil((TorsionBarCount - 2 * (HorizontalTorsionBarCount + VerticalTorsionBarCount) - 4) / 2);
		}
		TorsionBarCount = 2 * (HorizontalTorsionBarCount + VerticalTorsionBarCount) - 4;
		auto Tn = TorsionBarDesignStrength(DataIN.fy, aoh, Stirrup_For_Torsion, DataIN.TorsionBarGrade, ph, TorsionBarCount, DataIN.TorsionBar);
		auto ReduceTn = 0.75 * Tn;

		// 除錯用
		//cout << Tn << endl;
		//cout << ReduceTn << endl;
		
		// 計算主筋
		auto MinMainBarSpace = min({ 4.0 / 3.0 * DataIN.dagg, BarDiameter(DataIN.MainBar), 1.0 });
		auto MaxMainBarSpace = min({ 900000 / DataIN.fy - 2.5 * DataIN.cc,720000 / DataIN.fy });
		auto MinMainBarQuantity = AsMinValue(DataIN.bw, DataIN.fc, DataIN.fy, EffectiveDepth);
		auto MaxMainBarQuantity = AsMaxValue(DataIN.bw, EffectiveDepth, Fyt(DataIN.MainBarGrade));
		int MinHorizontalMainBarCount = ceil((DataIN.bw - 2.0 * DataIN.cc - 2.0 * BarDiameter(DataIN.Stirrup) + MaxMainBarSpace) / (BarDiameter(DataIN.MainBar) + MaxMainBarSpace));
		int MaxHorizontalMainBarCount = floor((DataIN.bw - 2.0 * DataIN.cc - 2.0 * BarDiameter(DataIN.Stirrup) + MinMainBarSpace) / (BarDiameter(DataIN.MainBar) + MinMainBarSpace));
		int MinMainBarCount = ceil(MinMainBarQuantity / BarArea(DataIN.MainBar));
		int MaxMainBarCount = floor(MaxMainBarQuantity / BarArea(DataIN.MainBar));
		if (MinHorizontalMainBarCount < MinMainBarCount) {
		MinHorizontalMainBarCount = MinMainBarCount;
		}
	
		// 排列及計算
		// 撓曲筋
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

		// 剪力筋
		vector <double> Mpr1_EPSILON;
		vector <double> Mpr2_EPSILON;
		double Mpr1_COMPRESSIVESTRESSZONE;
		double Mpr2_COMPRESSIVESTRESSZONE;
		double Mpr1;
		double Mpr2;
		double ve;

		// 繫筋
		int TieCount;

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
			AllUpperCount = reduce(UpperEachLayerCount.begin(), UpperEachLayerCount.end(), 0);
			if (AllUpperCount > MaxMainBarCount) {
				cout << "Your section size for Amount of Reinforcement is insufficient!\nPlease reset the section." << endl;
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
			AllLowerCount = reduce(LowerEachLayerCount.begin(), LowerEachLayerCount.end(), 0);
			if (AllLowerCount > MaxMainBarCount) {
				cout << "Your section size for Amount of Reinforcement is insufficient!\nPlease reset the section." << endl;
			}

			// 除錯用
			//cout << AllLowerCount << endl;
				
			// 將每層筋支數合併為一個向量
			Mn2_PerLayerCount = UpperEachLayerCount;
			Mn2_PerLayerCount.insert(Mn2_PerLayerCount.end(), LowerEachLayerCount.rbegin(), LowerEachLayerCount.rend());
			// 計算每層有效深度
			UpperLayersDepthGuessC.resize(UpperLayers);
			LowerLayersDepthGuessC.resize(LowerLayers);
			UpperLayersDepthGuessC[0] = DataIN.cc + BarDiameter(DataIN.Stirrup) + (BarDiameter(DataIN.MainBar) / 2);
			LowerLayersDepthGuessC[0] = DataIN.h - DataIN.cc - BarDiameter(DataIN.Stirrup) - (BarDiameter(DataIN.MainBar) / 2);
			for (int layer = 1; layer <= UpperLayers - 1; layer++) {
				UpperLayersDepthGuessC[layer] = UpperLayersDepthGuessC[layer - 1] + BarDiameter(DataIN.MainBar) + 1;
			}
			for (int layer = 1; layer <= LowerLayers - 1; layer++) {
				LowerLayersDepthGuessC[layer] = LowerLayersDepthGuessC[layer - 1] - BarDiameter(DataIN.MainBar) - 1;
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
				Mn1_EffectiveDepthGuessC[i] = DataIN.h - Mn1_EffectiveDepthGuessC[i];
			}
			Mn1_PerLayerCount.resize(Mn2_PerLayerCount.size());
			Mn1_PerLayerCount = Mn2_PerLayerCount;
			reverse(Mn1_PerLayerCount.begin(), Mn1_PerLayerCount.end());
			// Mn- 參數
			Mn2_EPSILON.resize(Mn2_PerLayerCount.size());
			// 計算Mn+及Mn-
			Mn1 = Moment(DataIN.bw, DataIN.h, DataIN.fc, DataIN.fy, DataIN.MainBar, Mn1_EffectiveDepthGuessC, Mn1_PerLayerCount, DataIN.TorsionBar, HorizontalTorsionBarCount, false, Mn1_EPSILON, Mn1_COMPRESSIVESTRESSZONE);
			Mn2 = -Moment(beff, DataIN.h, DataIN.fc, DataIN.fy, DataIN.MainBar, Mn2_EffectiveDepthGuessC, Mn2_PerLayerCount, DataIN.TorsionBar, HorizontalTorsionBarCount, false, Mn2_EPSILON, Mn2_COMPRESSIVESTRESSZONE);
			// 計算Mpr+及Mpr-及Ve
			Mpr1_EPSILON = Mn1_EPSILON; Mpr2_EPSILON = Mn2_EPSILON;
			Mpr1 = Moment(DataIN.bw, DataIN.h, DataIN.fc, DataIN.fy, DataIN.MainBar, Mn1_EffectiveDepthGuessC, Mn1_PerLayerCount, DataIN.TorsionBar, HorizontalTorsionBarCount, true, Mpr1_EPSILON, Mpr1_COMPRESSIVESTRESSZONE);
			Mpr2 = -Moment(beff, DataIN.h, DataIN.fc, DataIN.fy, DataIN.MainBar, Mn2_EffectiveDepthGuessC, Mn2_PerLayerCount, DataIN.TorsionBar, HorizontalTorsionBarCount, true, Mpr2_EPSILON, Mpr2_COMPRESSIVESTRESSZONE);
			ve = Ve(DataIN.ln, Mpr1, Mpr2, DataIN.LoadFactorDL, DataIN.LoadFactorLL, Dl, Ll);
			
			// 計算撓曲折減係數
			if (DataIN.Si == true) {
				ReductionFactorMn1 = ReductionFactorMforSpiral(DataIN.fy, Mn1_EPSILON);
				ReductionFactorMn2 = ReductionFactorMforSpiral(DataIN.fy, Mn2_EPSILON);
			}
			else {
				ReductionFactorMn1 = ReductionFactorMforOther(DataIN.fy, Mn1_EPSILON);
				ReductionFactorMn2 = ReductionFactorMforOther(DataIN.fy, Mn2_EPSILON);
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
			if (ReduceMn1 >= DataIN.Mu1 && ReduceMn2 <= DataIN.Mu2) {
				if (UpperEachLayerCount[0] > LowerEachLayerCount[0]) {
					TieCount = UpperEachLayerCount[0] / 2 + 1;
					if (UpperEachLayerCount[0] % 2 != 0) {
						if (LowerEachLayerCount[0] > TieCount) {


							break;
						}
						else {
							LowerCount1 = TieCount;
						}
					}
					else {
						TieCount += TieCount % 2;
						if (LowerEachLayerCount[0] > TieCount) {
							if (LowerEachLayerCount[0] % 2 == 0) {
								break;
							}
							else {
								LowerCount1 += 1;
							}
						}
						else {
							LowerCount1 = TieCount;
						}
					}
				}
				else if (UpperEachLayerCount[0] < LowerEachLayerCount[0]) {
					TieCount = LowerEachLayerCount[0] / 2 + 1;
					if (LowerEachLayerCount[0] % 2 != 0) {
						if (UpperEachLayerCount[0] > TieCount) {
							break;
						}
						else {
							UpperCount1 = TieCount;
						}
					}
					else {
						TieCount += TieCount % 2;
						if (UpperEachLayerCount[0] > TieCount) {
							if (UpperEachLayerCount[0] % 2 == 0) {
								break;
							}
							else {
								UpperCount1 += 1;
							}
						}
						else {
							UpperCount1 = TieCount;
						}
					}
				}
				else {
					break;
				}
			}
			// 加支數區
			if (ReduceMn1 < DataIN.Mu1) {
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
			if (ReduceMn2 > DataIN.Mu2) {
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

		// 檢驗區
		cout << "撓曲筋最小水平支數 = " << MinHorizontalMainBarCount << endl
			<< "撓曲筋最大水平支數 = " << MaxHorizontalMainBarCount << endl
			<< "撓曲筋最小總支數 = " << MinMainBarCount << endl
			<< "撓曲筋最大總支數 = " << MaxMainBarCount << endl
			<< "  Mu+ = " << DataIN.Mu1 << endl << "  Mu- = " << DataIN.Mu2 << endl << "φMn+ = " << ReduceMn1 << endl << "φMn- = " << ReduceMn2 << endl;
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
		int TotalTorsionBarCount = 2 * (HorizontalTorsionBarCount + VerticalTorsionBarCount) - 4;
		cout << "扭矩筋水平支數 = " << HorizontalTorsionBarCount << endl
			<< "扭矩筋垂直支數 = " << VerticalTorsionBarCount << endl
			<< "扭矩筋總支數 = " << TotalTorsionBarCount << endl;
		cout <<  "  Tu = " << DataIN.Tu << endl << "φTn = " << ReduceTn << endl;
		DataOUT.Mu1 = DataIN.Mu1; DataOUT.Mu2 = DataIN.Mu2; DataOUT.Vu1 = DataIN.Vu1; DataOUT.Vu2 = DataIN.Vu2; DataOUT.Tu = DataIN.Tu;
		DataOUT.Sw = DataIN.Sw; DataOUT.bw = DataIN.bw; DataOUT.hf = DataIN.hf; DataOUT.h = DataIN.h; DataOUT.bf = DataIN.bf; DataOUT.ln = DataIN.ln;
		DataOUT.fc = DataIN.fc; DataOUT.fy = DataIN.fy;
		DataOUT.Dl = DataIN.Dl; DataOUT.Ll = DataIN.Ll; DataOUT.LoadFactorDL = DataIN.LoadFactorDL; DataOUT.LoadFactorLL = DataIN.LoadFactorLL;
		DataOUT.WingWidthType = DataIN.WingWidthType; DataOUT.WidthType = DataIN.WidthType;
		DataOUT.Ei = DataIN.Ei; DataOUT.EarthquakeArea = DataIN.EarthquakeArea;
		DataOUT.MainBar = DataIN.MainBar; DataOUT.Stirrup = DataIN.Stirrup; DataOUT.TorsionBar = DataIN.TorsionBar;
		DataOUT.cc = DataIN.cc;
		DataOUT.Si = DataIN.Si; DataOUT.StirrupInformation = DataIN.StirrupInformation;
		DataOUT.MainBarGrade = DataIN.MainBarGrade; DataOUT.StirrupGrade = DataIN.StirrupGrade; DataOUT.TorsionBarGrade = DataIN.TorsionBarGrade; DataOUT.dagg = DataIN.dagg;
		DataOUT.ForceMultLength = DataIN.ForceMultLength; DataOUT.Force = DataIN.Force; DataOUT.Length = DataIN.Length;
		DataOUT.ReduceMn1 = ReduceMn1; DataOUT.ReduceMn2 = ReduceMn2; DataOUT.ReductionFactorMn1 = ReductionFactorMn1; DataOUT.ReductionFactorMn2 = ReductionFactorMn2;
		DataOUT.MinHorizontalMainBarCount = MinHorizontalMainBarCount; DataOUT.MaxHorizontalMainBarCount = MaxHorizontalMainBarCount; DataOUT.MinMainBarCount = MinMainBarCount; DataOUT.MaxMainBarCount = MaxMainBarCount;
		DataOUT.Mn1_PerLayerCount = Mn1_PerLayerCount; DataOUT.Mn2_PerLayerCount = Mn2_PerLayerCount; DataOUT.Mn1_EffectiveDepthGuessC = Mn1_EffectiveDepthGuessC; DataOUT.Mn2_EffectiveDepthGuessC = Mn2_EffectiveDepthGuessC;
		DataOUT.ReduceTn = ReduceTn; DataOUT.ReduceT = 0.75; DataOUT.HorizontalTorsionBarCount = HorizontalTorsionBarCount; DataOUT.VerticalTorsionBarCount = VerticalTorsionBarCount; DataOUT.TotalTorsionBarCount = TotalTorsionBarCount;


	}
	else {
		// Reinforcement
	}

	return DataOUT;
}