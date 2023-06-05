// v 0.23.6.5

#define _WIN32_WINNT 0x0600
#pragma warning( disable : 4267 4244 26495)

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "crow.h"
#include  <sstream>

using namespace std;

// This program access location https://github.com/ER0T1/AutoRC

/*
#define _WIN32_WINNT_NT4                    0x0400 // Windows NT 4.0
#define _WIN32_WINNT_WIN2K                  0x0500 // Windows 2000
#define _WIN32_WINNT_WINXP                  0x0501 // Windows XP
#define _WIN32_WINNT_WS03                   0x0502 // Windows Server 2003
#define _WIN32_WINNT_WIN6                   0x0600 // Windows Vista
#define _WIN32_WINNT_VISTA                  0x0600 // Windows Vista
#define _WIN32_WINNT_WS08                   0x0600 // Windows Server 2008
#define _WIN32_WINNT_LONGHORN               0x0600 // Windows Vista
#define _WIN32_WINNT_WIN7                   0x0601 // Windows 7
#define _WIN32_WINNT_WIN8                   0x0602 // Windows 8
#define _WIN32_WINNT_WINBLUE                0x0603 // Windows 8.1
#define _WIN32_WINNT_WINTHRESHOLD           0x0A00 // Windows 10
#define _WIN32_WINNT_WIN10                  0x0A00 // Windows 10
*/
// https://learn.microsoft.com/zh-tw/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-170
// use C++20 to compile to avoid various warnings... 

// vcpkg install crow
// API documentation https://crowcpp.org/master/reference/classcrow_1_1query__string.html

const auto port = 18080;

// 出圖用全域變數
double enlarge = 15; // 放大倍率

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

double BarDiameter(int);
double BarArea(int);
bool Core(struct InPutData);
string ToSVG(double limmitOfB, double limmitOfH, double Offset, double h, double bw, double clearcoverToCenter, int MainBarCount, double MainBarR, double, double, double);

int main() {
	InPutData DataIN;

	// Read Strength Information:
	DataIN.Mu1 = 158.507, DataIN.Mu2 = -308.283, DataIN.Vu1 = 67.4, DataIN.Vu2 = -71.41, DataIN.Tu = 58.153; // kips-ft, kips, kips-ft
	// Read Size Information:
	DataIN.Sw = 260.0, DataIN.bw = 21.0, DataIN.hf = 12.0, DataIN.h = 28.0, DataIN.bf = 84, DataIN.ln = 236.0; // in
	// Read Material Information:
	DataIN.fc = 5000.0, DataIN.fy = 60000.0; // psi
	// Coefficient Load Combination:// psi/ft^3
	DataIN.Dl = 150.0, DataIN.Ll = 40.0; DataIN.LoadFactorDL = 1.2; DataIN.LoadFactorLL = 1.0;

	// Beam Type Confirmation
	cout << "Whether it is the Middle Beam or the Side Beam? (S/M)" << endl;
	while (true) {
		DataIN.WidthType = "s";
		if (DataIN.WidthType == "S" || DataIN.WidthType == "s" || DataIN.WidthType == "Side" || DataIN.WidthType == "side" || DataIN.WidthType == "邊梁" || DataIN.WidthType == "邊") {
			cout << "It is Side Beam." << endl;
			DataIN.WingWidthType = true;
			break;
		}
		else if (DataIN.WidthType == "M" || DataIN.WidthType == "m" || DataIN.WidthType == "Middle" || DataIN.WidthType == "middle" || DataIN.WidthType == "中間梁" || DataIN.WidthType == "中間" || DataIN.WidthType == "中") {
			cout << "It is Middle Beam." << endl;
			DataIN.WingWidthType = false;
			break;
		}
		else {
			cout << "Please answer correctly!\nWhether it is the Middle Beam or the Side Beam? (S/M)" << endl;
		}
	}

	//Strong Earthquake Area Confirmation
	cout << "Is it designed for strong earthquake areas? (Y/N)" << endl;
	while (true) {
		DataIN.EarthquakeArea = "y";
		if (DataIN.EarthquakeArea == "Y" || DataIN.EarthquakeArea == "y" || DataIN.EarthquakeArea == "Yes" || DataIN.EarthquakeArea == "yes" || DataIN.EarthquakeArea == "是") {
			DataIN.Ei = true;
			cout << "It is designed for strong earthquake areas." << endl;
			break;
		}
		else if (DataIN.EarthquakeArea == "N" || DataIN.EarthquakeArea == "n" || DataIN.EarthquakeArea == "No" || DataIN.EarthquakeArea == "no" || DataIN.EarthquakeArea == "否") {
			DataIN.Ei = false;
			cout << "It is'nt designed for strong earthquake areas." << endl;
			break;
		}
		else {
			cout << "Please answer correctly!\nIs it designed for strong earthquake areas? (Y/N)" << endl;
		}
	}

	//Enter the Rebar number
	cout << "Please enter the Longitudinal Reinforcement Number." << endl;
	DataIN.MainBar = 8;
	cout << "You use No." << DataIN.MainBar << " rebar as the Longitudinal Reinforcement." << endl;

	cout << "Please enter the Stirrup Number." << endl;
	DataIN.Stirrup = 6;
	cout << "You use No." << DataIN.Stirrup << " rebar as the Stirrup." << endl;

	cout << "Please enter the Torsion Bars Number." << endl;
	DataIN.TorsionBar = 6;
	cout << "You use No." << DataIN.TorsionBar << " rebar as the Torsion Bars." << endl;

	// Design Clear Cover thickness
	cout << "Please enter the Clear Cover thickness in inch." << endl;
	bool ClearCoverDesign = true;
	while (ClearCoverDesign) {
		DataIN.cc = 1.5;
		if (DataIN.cc > 4) {
			cout << "Please enter the correct value!\nThe thickness of Clear Cover must not exceed 4 inches!\nIt is recommended to set the cover thickness between 1.5 and 3 inches.\nPlease enter the Clear Cover thickness in inch." << endl;
		}
		else if (DataIN.cc > 3) {
			cout << "It is recommended to set the cover thickness between 1.5 and 3 inches.\nWould you like to change the design? (Y/N)" << endl;
			string ChangeClearCover;
			while (bool ChangeClearCoverF = true) {
				cin >> ChangeClearCover;
				if (ChangeClearCover == "Y" || ChangeClearCover == "y" || ChangeClearCover == "Yes" || ChangeClearCover == "yes" || ChangeClearCover == "是") {
					cout << "Please enter the Clear Cover thickness in inch." << endl;
					ChangeClearCoverF = false;
				}
				else if (ChangeClearCover == "N" || ChangeClearCover == "n" || ChangeClearCover == "No" || ChangeClearCover == "no" || ChangeClearCover == "否") {
					cout << "Clear Cover thickness is " << DataIN.cc << " inches." << endl;
					ChangeClearCoverF = false;
					ClearCoverDesign = false;
				}
				else {
					cout << "Please answer correctly!\nWould you like to change the design? (Y/N)" << endl;
				}
			}
		}
		else if (DataIN.cc < 1.5) {
			cout << "Please enter the correct value!\nThe thickness of Clear Cover must be no less than 1.5 inches!\nIt is recommended to set the cover thickness between 1.5 and 3 inches.\nPlease enter the Clear Cover thickness in inch." << endl;
		}
		else {
			cout << "Clear Cover thickness is " << DataIN.cc << " inches." << endl;
			ClearCoverDesign = false;
		}
	}

	//Stirrup Information
	DataIN.StirrupInformation = "n";
	cout << "Whether it is designed for Spiral Stirrup? (Y/N)" << endl;
	while (true) {
		if (DataIN.StirrupInformation == "Y" || DataIN.StirrupInformation == "y" || DataIN.StirrupInformation == "Yes" || DataIN.StirrupInformation == "yes" || DataIN.StirrupInformation == "是") {
			DataIN.Si = true;
			cout << "It is designed for Spiral Stirrup." << endl;
			break;
		}
		else if (DataIN.StirrupInformation == "N" || DataIN.StirrupInformation == "n" || DataIN.StirrupInformation == "No" || DataIN.StirrupInformation == "no" || DataIN.StirrupInformation == "否") {
			DataIN.Si = false;
			cout << "It is'nt designed for Spiral Stirrup." << endl;
			break;
		}
		else {
			cout << "Please answer correctly!\nWhether it is designed for Spiral Stirrup? (Y/N)" << endl;
		}
	}

	// Global Variable
	DataIN.MainBarGrade = 60.;
	DataIN.StirrupGrade = 60.;
	DataIN.TorsionBarGrade = 60.;
	DataIN.dagg = 3.0 / 4.0;

	bool EXE = Core(DataIN);

	// 出圖
	double h = DataIN.h * enlarge, bw = DataIN.bw * enlarge, limmitOfH = h + 50, limmitOfB = bw + 50;
	int MainBarCount = 7;
	double MainBarR = BarDiameter(DataIN.MainBar) / 2. * enlarge, StirrupOfD = BarDiameter(DataIN.Stirrup) * enlarge;
	//需運算之值
	double clearCover = DataIN.cc * enlarge, clearcoverToCenter = clearCover + MainBarR;
	double centerToCenter = (bw - 2 * clearcoverToCenter) / (MainBarCount - 1);
	const auto Offset = 10;

	crow::SimpleApp app;
	crow::mustache::set_global_base("web");

	CROW_ROUTE(app, "/")([]() {
		auto Page = crow::mustache::load_text("HTMLPage.htm");
		return Page;
		});

	CROW_ROUTE(app, "/StyleSheet.css")([]() {
		auto StyleSheetPage = crow::mustache::load_text("StyleSheet.css");
		return StyleSheetPage;
		});

	CROW_ROUTE(app, "/InPut").methods(crow::HTTPMethod::POST)([](const crow::request& req) {
		cout << req.body << endl; // expect JSON
		auto data = crow::json::load(req.body);
		// getting all the keys
		for (auto& k : data.keys()) cout << k << " ";
		auto name = string(data["name"]);
		auto age = int(data["age"]);
		auto boo = bool(data["boo"]);
		cout << "\nname: " << name << "\nage: " << age << "\nboo: " << (boo ? "true" : "false") << endl << "in array: ";
		for (auto& i : data["array"]) cout << int(i) << ", ";
		cout << "\nin obj: ";
		for (auto& val : data["obj"]) cout << "\n\t" << val.key() << ":" << val;
		cout << endl;

		return "OK!";
		});

	auto OutPut = ToSVG(limmitOfB, limmitOfH, Offset, h, bw, clearcoverToCenter, MainBarCount, MainBarR, centerToCenter, clearCover, StirrupOfD);
	cout << OutPut << endl;

	app.port(port).run();

	return 0;
}

string ToSVG(double limmitOfB, double limmitOfH, double Offset, double h, double bw, double clearcoverToCenter, int MainBarCount, double MainBarR, double centerToCenter, double clearCover, double StirrupOfD) {
	stringstream file;
	file << "<svg width= \" " << limmitOfB << " \" height = \" " << limmitOfH << " \"> \n";

	file << "<rect x = '" << Offset << "' y = '" << Offset << "' width = \" " << bw << " \" height = \" "
		<< h << " \" style = \"fill:rgb(255, 255, 255);stroke-width:2;stroke:rgb(0,0,0)\" /> \n";


	double cx1 = clearcoverToCenter, cy1 = clearcoverToCenter;
	for (int i = 1; i <= MainBarCount; i = i + 1)
	{
		file << "<circle cx = \" " << cx1 + Offset << " \" cy = \" " << cy1 + Offset << " \" r = \" "
			<< MainBarR << " \" stroke = \"black\" stroke-width = \"2\" fill = \"white\" /> \n";
		cx1 = cx1 + centerToCenter;
	}

	double cx2 = clearcoverToCenter, cy2 = h - clearcoverToCenter;
	for (int i = 1; i <= MainBarCount; i = i + 1)
	{
		file << "<circle cx = \" " << cx2 + Offset << " \" cy = \" " << cy2 + Offset << " \" r = \" "
			<< MainBarR << " \" stroke = \"black\" stroke-width = \"2\" fill = \"white\" /> \n";
		cx2 = cx2 + centerToCenter;
	}


	/*新箍筋
	<rect x = "50" y = "20" rx = "20" ry = "20" width = "150" height = "150"
	 style = "fill:red;stroke:black;stroke-width:5;opacity:0.5" / >*/
	for (int i = 0; i <= 1; i++) {
		file << "<rect x = \"" << clearCover + Offset - i * StirrupOfD << " \" y = \"" << clearCover + Offset - i * StirrupOfD << " \" rx = \"" << MainBarR + i * StirrupOfD << "\" ry = \" " << MainBarR + i * StirrupOfD << "\" width = \"" << bw - 2 * clearCover + i * 2 * StirrupOfD << "\" height = \" " << h - 2 * clearCover + i * 2 * StirrupOfD << "\"";
		file << "style = \"fill:none;stroke:black;stroke-width:2;opacity:1\"/>";
	}


	file << "</svg>\n";
	file << "<p>" << MainBarCount << " N0. ?" << " for main reinforcement " << "</p>";
	file << "</html>";
	//file.close();
	return file.str();
}

