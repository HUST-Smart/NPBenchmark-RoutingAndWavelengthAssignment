#include "Simulator.h"


using namespace std;
using namespace szx;


int main(int argc,char *argv[]) {
    //Simulator::initDefaultEnvironment();

    Simulator sim;
	String netPath;
	String trfPath;
    //sim.debug();
   // sim.benchmark(1);
	sim.parallelBenchmark(1);


	///算例生成
	//for (auto i = 3; i <= 5; ++i) {// 生成set Y 算例
	//	for (auto j = 20; j <= 100; j=j+20) {
	//		for (auto seed = 1; seed <= 5; ++seed) {
	//			trfPath =  "Y." + to_string(i) + "." + to_string(j) + "_seed=" + to_string(seed);
	//			netPath = "Y." + to_string(i) + "_seed=" + to_string(seed);
	//			sim.convertYandZInstance(trfPath,netPath,"table1/");
	//		}
	//	}
	//}

	//for (auto i = 20; i <= 100; i=i+20) {
	//	netPath = "Z.4x25";
	//	trfPath = netPath + "." + to_string(i);
	//	sim.convertYandZInstance(trfPath, netPath, "table2/");

	//	netPath = "Z.5x20";
	//	trfPath = netPath + "." + to_string(i);
	//	sim.convertYandZInstance(trfPath, netPath, "table2/");

	//	netPath = "Z.6x17";
	//	trfPath = netPath + "." + to_string(i);
	//	sim.convertYandZInstance(trfPath, netPath, "table2/");

	//	netPath = "Z.8x13";
	//	trfPath = netPath + "." + to_string(i);
	//	sim.convertYandZInstance(trfPath, netPath, "table2/");

	//	netPath = "Z.10x10";
	//	trfPath = netPath + "." + to_string(i);
	//	sim.convertYandZInstance(trfPath, netPath, "table2/");
	//}

	///*trfPath = "Y." + to_string(0) + "." + to_string(0) + "_seed=" + to_string(0);
	//netPath = "Y." + to_string(0) + "_seed=" + to_string(0);
	//sim.convertYandZInstance(trfPath, netPath,"table1/");*/


	//trfPath = "ATT";
	//sim.convertYandZInstance(trfPath, trfPath, "table3/");

	//trfPath = "ATT2";
	//sim.convertYandZInstance(trfPath, trfPath, "table3/");

	//trfPath = "brasil";
	//sim.convertYandZInstance(trfPath, trfPath, "table3/");

	//trfPath = "EON";
	//sim.convertYandZInstance(trfPath, trfPath, "table3/");

	//trfPath = "Finland";
	//sim.convertYandZInstance(trfPath, trfPath, "table3/");

	/*int index[4] = { 1,3,12,48 };

	netPath = "NSF";
	for (auto i = 0; i < 4; ++i) {
		trfPath = netPath + "."+to_string(index[i]);
		sim.convertYandZInstance(trfPath, netPath, "table3/");

	}


	netPath = "NSF2";
	for (auto i = 0; i < 4; ++i) {
		trfPath = netPath + "." + to_string(index[i]);
		
		sim.convertYandZInstance(trfPath, netPath, "table3/");

	}*/

    return 0;
}
