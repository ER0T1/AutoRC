#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include  <sstream>
#include "AutoRC.h"
using namespace std;

void SVG::rect(double x, double y,double b,double h,double rx, double ry ) {
	file << "<rect x = \"" << x << " \" y = \"" << y << " \" rx = \"" << rx << "\" ry = \" " << ry << "\" width = \"" 
		<< b << "\" height = \" " << h << "\""<< "style = \"fill:none;stroke:black;stroke-width:2;opacity:1\"/> \n";
}
void SVG::circle(double x,double y,double R ) {
	file << "<circle cx = \" " << x << " \" cy = \" " << y << " \" r = \" "
		<< R << " \" stroke = \"black\" stroke-width = \"2\" fill = \"white\" /> \n";
}
void SVG::line(double x1, double y1, double x2, double y2) {
	file << "<line x1=\" " << x1 << "\" y1=\" " << y1 << " \" x2=\" " << x2 << "\" y2=\" " << y2 << "\" style=\"stroke:rgb(0, 0, 0); stroke - width:2\" />";
}
void SVG::pathHead(double x, double y) {
	file << "<path d = \"M" << x << " " << y;
}
void SVG::pathLine(double x, double y) {
	file << "l" << x << " " << y;
}
void SVG::pathArc(double r1,double r2, double k, double x, double y) {
	file << "A" << r1 << " " << r2 << " " << 0 << " " << 0 << " " << k << " " << x << " " << y;
}
void SVG::pathZ() {
	file << "Z";
}
void SVG::pathTail() {
	file << "\" fill = \"white\"></path>";
}
void SVG::transHead(double x, double y) {
	file << "<g transform = \"translate(" << x << "," << y << ")\" stroke = \"#000\" stroke-width = \"2\">";
}
void SVG::transTail() {
	file << "</g>";
}
void SVG::Uhook(double i) {
	double k = 0;
	if (i == -1.0) k = 1;
	//file << "<g transform = \"translate(" << cx1 + Offset + k * (bw - 2 * clearcoverToCenter) << "," << cy1 + Offset << ")\" stroke = \"#000\" stroke-width = \"2\">";
	transHead(clearcoverToCenter + Offset + k * (bw - 2 * clearcoverToCenter), clearcoverToCenter + Offset);
	//file << "<path d = \"M" << 0 << " " << -R1 << "A" << R1 * i << " " << R1 << " " << 0 << " " << 0 << " " << 1 - k << " " << hook1 * i << " " << -hook1 << "l" << hook2 * i << " " << hook2 << "l" << -hook3 * i << " " << hook3 << "l" << -hook2 * i << " " << -hook2 << "\" fill = \"white\"></path>";
	pathHead(0, -R1); 
	pathArc(R1 * i, R1, 1 - k, hook1 * i, -hook1); pathLine(hook2 * i, hook2); pathLine(-hook3 * i, hook3); pathLine(-hook2 * i, -hook2);
	pathTail();
	//file << "<path d = \"M" << -sqrt(MainBarR * MainBarR / 2) * i << " " << sqrt(MainBarR * MainBarR / 2) << "l" << hook2 * i << " " << hook2 << "l" << -hook3 * i << " " << hook3 << "l" << (-hook2 + sqrt(StirrupOfD * StirrupOfD / 2)) * i << " " << -hook2 + sqrt(StirrupOfD * StirrupOfD / 2) << "\" fill = \"white\"></path>";
	pathHead(-sqrt(MainBarR * MainBarR / 2) * i, sqrt(MainBarR * MainBarR / 2));
	pathLine(hook2 * i, hook2); pathLine(-hook3 * i, hook3); pathLine((-hook2 + sqrt(StirrupOfD * StirrupOfD / 2)) * i, -hook2 + sqrt(StirrupOfD * StirrupOfD / 2)); 
	pathTail();
	//file << "</g>";
	transTail();
}
void SVG::leg(double x, int i) {
	double k = 1;
	double k1 = 0;
	if (i == -1) k = 0, k1 = 1;

	//file << "<g transform = \"translate(" << x << "," << y << ")\" stroke = \"#000\" stroke-width = \"2\">";
	transHead(x, h / 2 + Offset);

	//file << "<path d = \"M" << -R1 << " " << i * (-h / 2 + clearcoverToCenter) << "A" << R1 << " " << R1 << " " << 0 << " " << 0 << " " << k << " " << hook1 << " " << i * (-hook1 - h / 2 + clearcoverToCenter) << "l" << hook2 << " " << i * hook2 << "l" << -hook3 << " " << i * hook3 << "l" << -hook2 << " " << i * -hook2;
	pathHead(-R1, i * (-h / 2 + clearcoverToCenter)); pathArc(R1, R1, k, hook1, i * (-hook1 - h / 2 + clearcoverToCenter)); pathLine(hook2, i * hook2); pathLine(-hook3, i * hook3); pathLine(-hook2, i * -hook2);
	//file << "A" << MainBarR << " " << MainBarR << " " << 0 << " " << 0 << " " << k1 << " " << -MainBarR << " " << i * (-h / 2 + clearcoverToCenter) << "l" << 0 << " " << i * (h - 2 * clearcoverToCenter) << "A" << MainBarR << " " << MainBarR << " " << 0 << " " << 0 << " " << k1 << " " << 0 << " " << i * (1. / 2. * h - clearcoverToCenter + MainBarR);
	pathArc(MainBarR, MainBarR, k1, -MainBarR, i * (-h / 2 + clearcoverToCenter)); pathLine(0, i * (h - 2 * clearcoverToCenter)); pathArc(MainBarR, MainBarR, k1, 0, i * (1. / 2. * h - clearcoverToCenter + MainBarR));
	//file << "l" << 3 * enlarge << " " << 0 << "l" << 0 << " " << i * StirrupOfD << "l" << -3 * enlarge << " " << 0 << "A" << R1 << " " << R1 << " " << 0 << " " << 0 << " " << k << " " << -R1 << " " << i * (1. / 2. * h - clearcoverToCenter) << "Z";
	pathLine(3 * enlarge, 0); pathLine(0, i * StirrupOfD); pathLine(-3 * enlarge, 0); pathArc(R1, R1, k, -R1, i * (1. / 2. * h - clearcoverToCenter)); pathZ();
	//file << "\" fill = \"white\"></path>";
	pathTail();

	//file << "</g>";
	transTail();
}
void SVG::SVGbeamSide() {
	SVGHead(Ln_side + 50, h_side + 50);

	rect(Offset, Offset, Ln_side, h_side);
	for (int i = 0; i < 2; i++) {
		//file << "<line x1=\" " << Offset << "\" y1=\" " << Offset + clearcoverToCenter + i * (h - 2 * clearcoverToCenter) << " \" x2=\" " << Offset + Ln_side
		line(Offset, Offset + clearcoverToCenter_side + i * (h_side - 2 * clearcoverToCenter_side), Offset + Ln_side, Offset + clearcoverToCenter_side + i * (h_side - 2 * clearcoverToCenter_side));
			//<< "\" y2=\" " << Offset + clearcoverToCenter + i * (h - 2 * clearcoverToCenter) << "\" style=\"stroke:rgb(0, 0, 0); stroke - width:2\" />";
	}

	if (sideCase) {
		for (int i = 0; i < StirrupAccount; i++) {
			//file << "<line x1=\"" << Offset + place1st + i * s << "\" y1=\" " << Offset + clearcoverToCenter
				//<< " \" x2=\" " << Offset + place1st + i * s << "\" y2=\"" << Offset + h - clearcoverToCenter << "\" style=\"stroke:rgb(0, 0, 0); stroke - width:2\" />";
			line(Offset + place1st + i * s, Offset + clearcoverToCenter_side, Offset + place1st + i * s, Offset + h_side - clearcoverToCenter_side);
		}
	}
	else {
		for (int i = 0; i < 2; i++) {
			//file << "<line x1=\" " << Offset + 2 * enlarge2 + i * (Ln_side - 4 * enlarge2) << "\" y1=\" " << Offset + clearcoverToCenter_side
				//<< " \" x2=\" " << Offset + 2 * enlarge2 + i * (Ln_side - 4 * enlarge2) << "\" y2=\" " << Offset + h_side - clearcoverToCenter_side << "\" style=\"stroke:rgb(0, 0, 0); stroke - width:2\" />";
			line(Offset + 2 * enlarge2 + i * (Ln_side - 4 * enlarge2), Offset + clearcoverToCenter_side, Offset + 2 * enlarge2 + i * (Ln_side - 4 * enlarge2), Offset + h_side - clearcoverToCenter_side);
		}
		for (int i = 0; i < StirrupAccount - 2; i++) {
			//file << "<line x1=\"" << Offset + place1st + i * s << "\" y1=\" " << Offset + clearcoverToCenter_side
				//<< " \" x2=\" " << Offset + place1st + i * s << "\" y2=\"" << Offset + h_side - clearcoverToCenter_side << "\" style=\"stroke:rgb(0, 0, 0); stroke - width:2\" />";
			line(Offset + place1st + i * s, Offset + clearcoverToCenter_side, Offset + place1st + i * s, Offset + h_side - clearcoverToCenter_side);
		}
	}

	SVGTail();
}
void SVG::SVGbeamSection() {
	SVGHead(bw + 50, h + 50);
	rect(Offset, Offset, bw, h);

	double cx1 = clearcoverToCenter, cy1 = clearcoverToCenter;
	for (int i = 0; i < MainBarCount; i++) {
		if (ifLeaderOnTop == false && MainBarDepend_H[i] == false) {
			cx1 = cx1 + centerToCenter;
			continue;
		}
		circle(cx1 + Offset, cy1 + Offset, MainBarR);
		cx1 = cx1 + centerToCenter;
	}

	double cx2 = clearcoverToCenter, cy2 = h - clearcoverToCenter;
	for (int i = 0; i < MainBarCount; i++) {
		if (ifLeaderOnTop == true && MainBarDepend_H[i] == false) {
			cx2 = cx2 + centerToCenter;
			continue;
		}
		circle(cx2 + Offset, cy2 + Offset, MainBarR);
		cx2 = cx2 + centerToCenter;
	}

	// Test 
	/*double cx1 = clearcoverToCenter, cy1 = clearcoverToCenter;
	for (int i = 1; i <= MainBarCount; i++) {
		circle(cx1 + Offset, cy1 + Offset, MainBarR);
		cx1 = cx1 + centerToCenter;
	}
	double cx2 = clearcoverToCenter, cy2 = h - clearcoverToCenter;
	for (int i = 1; i <= MainBarCount; i = i + 1){
		circle(cx2 + Offset, cy2 + Offset, MainBarR);
		cx2 = cx2 + centerToCenter;
	}*/

	//stirrup
	for (int i = 0; i <= 1; i++) {
		rect(clearCover + Offset - i * StirrupOfD, clearCover + Offset - i * StirrupOfD, bw - 2 * clearCover + i * 2 * StirrupOfD, h - 2 * clearCover + i * 2 * StirrupOfD, MainBarR + i * StirrupOfD, MainBarR + i * StirrupOfD);
	}
	//stirrup hook
	Uhook(1.0);
	Uhook(-1.0);
	//leg
	double UpOrDown = 1.0;
	for (int i = 1; i < MainBarCount - 1; i++) {
		if (legs_H[i] == true) {
			leg(clearcoverToCenter + Offset + i * centerToCenter, UpOrDown);
			UpOrDown *= -1.0;
		}
		
	}
	//leg(clearcoverToCenter + Offset + 4 * centerToCenter, -1);
	//leg(clearcoverToCenter + Offset + 2 * centerToCenter, 1);

	SVGTail();
}
SVG::SVG(const struct RcData& d) {
	//Read the output data and then calculate the parameter for drawing
	h = d.h * enlarge;
	bw = d.bw * enlarge;
	MainBarCount = d.leading;//Using leading
	MainBarR = BarDiameter(d.MainBar) / 2. * enlarge; 
	StirrupOfD = BarDiameter(d.Stirrup) * enlarge;
	clearCover = d.cc * enlarge;
	clearcoverToCenter = clearCover + MainBarR;
	centerToCenter = (bw - 2. * clearcoverToCenter) / (MainBarCount - 1.);
	ifLeaderOnTop = d.leader;
	MainBarDepend_H = d.Coordinate.MainBarDepend_H;
	legs_H = d.Coordinate.legs_H;
	//hook
	R1 = StirrupOfD + MainBarR;
	hook1 = hook1 = sqrt(R1 * R1 / 2);
	hook2 = sqrt(9 * enlarge * enlarge / 2);
	hook3 = sqrt(StirrupOfD * StirrupOfD / 2);
	//side
	s = 5. * enlarge2;//need to be chage !!!
	Ln_side = d.ln * enlarge2;
	h_side = d.h * enlarge2;
	clearcoverToCenter_side = clearcoverToCenter / enlarge * enlarge2;
	//side//compute
	if (Ln_side - floor(Ln_side / s) * s <= 4.0) {
		sideCase = true;
		StirrupAccount = ceil(Ln_side / s);
		place1st = (Ln_side - (StirrupAccount - 1) * s) / 2.0;
	}
	else { 
		sideCase = false; 
		StirrupAccount = ceil(Ln_side / s) + 1;
		place1st = (Ln_side - (StirrupAccount - 3) * s) / 2.0;
	}
}


//int main() {
//	RcData Data;
//	SVG test(Data);
//	test.SVGbeamSection();//process
//	test.SVGbeamSide();//process
//	cout << test.output();//final sesult
//}