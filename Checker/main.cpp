#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <limits>
#include <set>

#include "../Solver/PbReader.h"
#include "../Solver/RWA.pb.h"


using namespace std;
using namespace pb;


int main(int argc, char *argv[]) {
	enum CheckerFlag {
		IoError = 0x0,
		FormatError = 0x1,
		RoutingError = 0x2,
		WaveLenConflictError = 0x4
	};


	string inputPath;
	string outputPath;

	if (argc > 1) {
		inputPath = argv[1];
	}
	else {
		cerr << "input path: " << flush;
		cin >> inputPath;
	}

	if (argc > 2) {
		outputPath = argv[2];
	}
	else {
		cerr << "output path: " << flush;
		cin >> outputPath;
	}

	pb::RWA::Input input;
	if (!load(inputPath, input)) { return ~CheckerFlag::IoError; }

	pb::RWA::Output output;
	ifstream ifs(outputPath);
	if (!ifs.is_open()) { return ~CheckerFlag::IoError; }
	string submission;
	getline(ifs, submission); // skip the first line.
	ostringstream oss;
	oss << ifs.rdbuf();
	jsonToProtobuf(oss.str(), output);

	if (output.traout_size() != input.traffics_size()) { return ~CheckerFlag::FormatError; }

	// check solution.
	int error = 0;
	// check constraints.
	
	for (auto o = output.traout().begin(); o != output.traout().end(); ++o) {
		int wave = (*o).wave();
		for (auto o2 = o + 1; o2 != output.traout().end(); ++o2) {
			int wave2 = (*o2).wave();
			if (wave == wave2) {
				for (auto p = (*o).path().begin(); p != (*o).path().end()-1; ++p) {
					int node1 = *p, node2 = *(p + 1);
					for (auto p2 = (*o2).path().begin(); p2 != (*o2).path().end()-1; ++p2) {
						int node3 = *p2, node4 = *(p2 + 1);
						if (node1 == node3 && node2 == node4) {
							error |= CheckerFlag::WaveLenConflictError;
						}
					}
				}
			}
		}
	}
	
	// check objective.
	set<int> waveLens;
	for (auto o = output.traout().begin(); o != output.traout().end(); ++o) {
		waveLens.insert((*o).wave());
	}
	int waveLenNum = static_cast<int>(waveLens.size());


	int returnCode = (error == 0) ? waveLenNum : ~error;
	cout << ((error == 0) ? waveLenNum : returnCode) << endl;
	return returnCode;
}