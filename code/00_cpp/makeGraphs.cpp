#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
using namespace std;

bool verboseOutput = true;

void verbose(string output) {
    if(verboseOutput)
        cout << output << endl << endl;
    return;
}


void makeGraphs() {

    cout << endl;

	// 1. User input file path
	string infile = "run12130_calo_qdc_tdc.txt";

//    // user inputs text file name
//	string fin;
//	cout << "Enter input file: ";
//	getline(cin, fin);\
//	cout << fin << "\n";
	
	
	// 2. Open file for reading
	ifstream fin;
    fin.open(infile);

	if (!fin) {
		cout << "Error: Could not open file." << endl;
		return;
	}
    verbose("File opened for reading.");
	
	
	// 3. Skip header
    int headerLines = 5;
	string line;
	for (int i = 0; i < headerLines; i++) {
		getline(fin, line);
	}
    verbose("Last line of header:\n\"" + line + "\"");
	
	
	// 4. Count number of events
	int numEvents = 0;
	while (getline(fin, line)) {
		numEvents++;
	}
    verbose("There are " + to_string(numEvents) + " events.");

    fin.close();
	
	
    // 5. Populate data arrays: [event_num][x-coord][y-coord]
    // 5.1. Reopen file and skip header

	ifstream fin2;
    fin2.open(infile);

	if (!fin2) {
		cout << "Error: Could not open file." << endl;
		return;
	}
    verbose("File opened for storing values.");

    for (int i = 0; i < headerLines; i++) {
		getline(fin2, line);
	}
	
	
	// 5.2. Instantiate TDC/QDC arrays and populate

	typedef float dimensions[8][8];
	dimensions *TDC = new dimensions[numEvents];
	dimensions *QDC = new dimensions[numEvents];

	int numArgs = 1 + 2*8*8;

	float data[numArgs];

	
	for (int i = 0; i < numEvents; i++) {
		getline(fin2, line);
		
		istringstream linereader(line, std::ios::binary);
		for (int j = 0; j < numArgs; j++) {
			linereader >> data[j];	
		}

		int thisEventNum = data[0];
		int ptr = 1;

		for (int x = 0; x < 8; x++) {
			for (int y = 0; y < 8; y++) {
				TDC[thisEventNum][x][y] = data[ptr++];
				QDC[thisEventNum][x][y] = data[ptr++];
			}
		}	
	}

	fin2.close();
    verbose("TDC and QDC arrays created.");
	
	
	// 6. Print QDC composite canvas
	
	int xMax = 1000;
	int resolution = 10;
	int nBins = xMax / resolution;

	TCanvas *QDC_canv = new TCanvas("QDC_canv", "QDC Bars (counts v channel)");
	QDC_canv -> Divide(8,8);

	TH1F *QDC_hist;
	QDC_hist = new TH1F[65];
	
	int row, col, index, maxBin, maxBinValue;
	double stats[5] = {0,0,0,0,0};
	string title;

	verbose("canvases created");

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {

			row = x;
			col = (-y+7);
			index = row+8*(col) + 1; // index of pad, referenced by "cd"

			verbose("(" + to_string(x) + ", " + to_string(y) + ")\nindex: " + to_string(index));

			// QDC_hist[index] = TH1F("h"+to_string(index), "("+to_string(x)+","+to_string(y)+")", nBins, 0, xMax);
			// QDC_hist[index] = TH1F("eek", "ak", nBins, 0, xMax);
			// TDC_hist[index-1] = TH1F("h", "h", nBins, 0, xMax);

			title = "bar ("+to_string(x)+","+to_string(y)+")";

			QDC_hist[index].SetName("aaa");
			QDC_hist[index].SetTitle(title.c_str());
			QDC_hist[index].SetBins(nBins, 0, xMax);	

			verbose("this histogram declared");

			for (int i = 1; i < numEvents; i++) {
				QDC_hist[index].Fill(QDC[i][x][y]);
				// QDC_hist -> Fill(QDC[i][x][y]);
			}

			verbose("populated");

			maxBin = 0;
			maxBinValue = QDC_hist[index].GetBinContent(0);
		
		    for (int i = 1; i < nBins; i++) {
				if (QDC_hist[index].GetBinContent(i) > maxBinValue) {
					maxBinValue = QDC_hist[index].GetBinContent(i);
					maxBin = i;
				}
			}
			
			QDC_hist[index].PutStats(stats); // reset mean value, etc
		
			for (int i = 1; i <= nBins - maxBin; i++) {
				QDC_hist[index].SetBinContent(i,QDC_hist[index].GetBinContent(i+maxBin));
			}
		
			for (int i = nBins-maxBin; i<= nBins; i++) {
				QDC_hist[index].SetBinContent(i,0);
			}

			QDC_canv -> cd(index);
			QDC_canv -> GetPad(index) -> SetLogy();
			QDC_hist[index].Draw();

			verbose("histogram drawn");	
		}
	}

	

	// 7. Print TDC composite canvas
	
	// int xMax = 1000;
	// int resolution = 10;
	// int nBins = xMax / resolution;
	
	TCanvas *TDC_canv = new TCanvas("TDC_canv", "TDC Bars (counts v channel)");
	TDC_canv -> Divide(8,8);

	TH1F *TDC_hist;
	TDC_hist = new TH1F[65];
	
	// int row, col, index, maxBin, maxBinValue;
	// double stats[5] = {0,0,0,0,0};
	// string title;

	verbose("canvases created");

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {

			row = x;
			col = (-y+7);
			index = row+8*(col) + 1; // index of pad, referenced by "cd"

			verbose("(" + to_string(x) + ", " + to_string(y) + ")\nindex: " + to_string(index));

			// QDC_hist[index] = TH1F("h"+to_string(index), "("+to_string(x)+","+to_string(y)+")", nBins, 0, xMax);
			// QDC_hist[index] = TH1F("eek", "ak", nBins, 0, xMax);
			// TDC_hist[index-1] = TH1F("h", "h", nBins, 0, xMax);

			title = "bar ("+to_string(x)+","+to_string(y)+")";

			TDC_hist[index].SetName("aaa");
			TDC_hist[index].SetTitle(title.c_str());
			TDC_hist[index].SetBins(40, -100, 300);	

			verbose("this histogram declared");
			
			for (int i = 1; i < numEvents; i++) {
				TDC_hist[index].Fill(TDC[i][x][y]);
				// QDC_hist -> Fill(QDC[i][x][y]);
			}

			verbose("populated");

			

			// maxBin = 0;
			// maxBinValue = TDC_hist[index].GetBinContent(0);
		
			// for (int i = 1; i < nBins; i++) {
			// 	if (TDC_hist[index].GetBinContent(i) > maxBinValue) {
			// 		maxBinValue = TDC_hist[index].GetBinContent(i);
			// 		maxBin = i;
			// 	}
			// }

			// TDC_hist[index].PutStats(stats); // reset mean value, etc
					
			// for (int i = 1; i <= nBins - maxBin; i++) {
			// 	TDC_hist[index].SetBinContent(i,TDC_hist[index].GetBinContent(i+maxBin));
			// }
		
			// for (int i = nBins-maxBin; i<= nBins; i++) {
			// 	TDC_hist[index].SetBinContent(i,0);
			// }

			TDC_canv -> cd(index);
			// TDC_canv -> GetPad(index) -> SetLogy();
			TDC_hist[index].Draw();

			verbose("histogram drawn");
			
		}
	}
}