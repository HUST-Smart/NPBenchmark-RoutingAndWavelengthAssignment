#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <algorithm>
#include <random>

#include <cstring>

#include "Simulator.h"
#include "ThreadPool.h"


using namespace std;


namespace szx {

	// EXTEND[szx][5]: read it from InstanceList.txt.
	static const vector<String> instList({
		"ATT",
		"ATT2",
		"brasil",
		"EON",
		"Finland",
		"NSF.1",
		"NSF.3",
		"NSF.12",
		"NSF.48",
		"NSF2.1",
		"NSF2.3",
		"NSF2.12",
		"NSF2.48",
		"Y.0.0_seed=0",
		"Y.3.20_seed=1",
		"Y.3.20_seed=2",
		"Y.3.20_seed=3",
		"Y.3.20_seed=4",
		"Y.3.20_seed=5",
		"Y.3.40_seed=1",
		"Y.3.40_seed=2",
		"Y.3.40_seed=3",
		"Y.3.40_seed=4",
		"Y.3.40_seed=5",
		"Y.3.60_seed=1",
		"Y.3.60_seed=2",
		"Y.3.60_seed=3",
		"Y.3.60_seed=4",
		"Y.3.60_seed=5",
		"Y.3.80_seed=1",
		"Y.3.80_seed=2",
		"Y.3.80_seed=3",
		"Y.3.80_seed=4",
		"Y.3.80_seed=5",
		"Y.3.100_seed=1",
		"Y.3.100_seed=2",
		"Y.3.100_seed=3",
		"Y.3.100_seed=4",
		"Y.3.100_seed=5",
		"Y.4.20_seed=1",
		"Y.4.20_seed=2",
		"Y.4.20_seed=3",
		"Y.4.20_seed=4",
		"Y.4.20_seed=5",
		"Y.4.40_seed=1",
		"Y.4.40_seed=2",
		"Y.4.40_seed=3",
		"Y.4.40_seed=4",
		"Y.4.40_seed=5",
		"Y.4.60_seed=1",
		"Y.4.60_seed=2",
		"Y.4.60_seed=3",
		"Y.4.60_seed=4",
		"Y.4.60_seed=5",
		"Y.4.80_seed=1",
		"Y.4.80_seed=2",
		"Y.4.80_seed=3",
		"Y.4.80_seed=4",
		"Y.4.80_seed=5",
		"Y.4.100_seed=1",
		"Y.4.100_seed=2",
		"Y.4.100_seed=3",
		"Y.4.100_seed=4",
		"Y.4.100_seed=5",
		"Y.5.20_seed=1",
		"Y.5.20_seed=2",
		"Y.5.20_seed=3",
		"Y.5.20_seed=4",
		"Y.5.20_seed=5",
		"Y.5.40_seed=1",
		"Y.5.40_seed=2",
		"Y.5.40_seed=3",
		"Y.5.40_seed=4",
		"Y.5.40_seed=5",
		"Y.5.60_seed=1",
		"Y.5.60_seed=2",
		"Y.5.60_seed=3",
		"Y.5.60_seed=4",
		"Y.5.60_seed=5",
		"Y.5.80_seed=1",
		"Y.5.80_seed=2",
		"Y.5.80_seed=3",
		"Y.5.80_seed=4",
		"Y.5.80_seed=5",
		"Y.5.100_seed=1",
		"Y.5.100_seed=2",
		"Y.5.100_seed=3",
		"Y.5.100_seed=4",
		"Y.5.100_seed=5",
		"Z.4x25.20",
		"Z.4x25.40",
		"Z.4x25.60",
		"Z.4x25.80",
		"Z.4x25.100",
		"Z.5x20.20",
		"Z.5x20.40",
		"Z.5x20.60",
		"Z.5x20.80",
		"Z.5x20.100",
		"Z.6x17.20",
		"Z.6x17.40",
		"Z.6x17.60",
		"Z.6x17.80",
		"Z.6x17.100",
		"Z.8x13.20",
		"Z.8x13.40",
		"Z.8x13.60",
		"Z.8x13.80",
		"Z.8x13.100",
		"Z.10x10.20",
		"Z.10x10.40",
		"Z.10x10.60",
		"Z.10x10.80",
		"Z.10x10.100",
});

	void Simulator::initDefaultEnvironment() {
		RWAsolver::Environment env;
		env.save(Env::DefaultEnvPath());
		RWAsolver::Configuration cfg;
	}

	void Simulator::run(const Task &task) {
		String instanceName(task.instSet + task.instId + ".json");
		String solutionName(task.instSet + task.instId + ".json");

		char argBuf[Cmd::MaxArgNum][Cmd::MaxArgLen];
		char *argv[Cmd::MaxArgNum];
		for (int i = 0; i < Cmd::MaxArgNum; ++i) { argv[i] = argBuf[i]; }
		strcpy(argv[ArgIndex::ExeName], ProgramName().c_str());

		int argc = ArgIndex::ArgStart;

		strcpy(argv[argc++], Cmd::InstancePathOption().c_str());
		strcpy(argv[argc++], (InstanceDir() + instanceName).c_str());

		System::makeSureDirExist(SolutionDir());
		strcpy(argv[argc++], Cmd::SolutionPathOption().c_str());
		strcpy(argv[argc++], (SolutionDir() + solutionName).c_str());

		if (!task.randSeed.empty()) {
			strcpy(argv[argc++], Cmd::RandSeedOption().c_str());
			strcpy(argv[argc++], task.randSeed.c_str());
		}

		if (!task.timeout.empty()) {
			strcpy(argv[argc++], Cmd::TimeoutOption().c_str());
			strcpy(argv[argc++], task.timeout.c_str());
		}

		if (!task.maxIter.empty()) {
			strcpy(argv[argc++], Cmd::MaxIterOption().c_str());
			strcpy(argv[argc++], task.maxIter.c_str());
		}

		if (!task.jobNum.empty()) {
			strcpy(argv[argc++], Cmd::JobNumOption().c_str());
			strcpy(argv[argc++], task.jobNum.c_str());
		}

		if (!task.runId.empty()) {
			strcpy(argv[argc++], Cmd::RunIdOption().c_str());
			strcpy(argv[argc++], task.runId.c_str());
		}

		if (!task.cfgPath.empty()) {
			strcpy(argv[argc++], Cmd::ConfigPathOption().c_str());
			strcpy(argv[argc++], task.cfgPath.c_str());
		}

		if (!task.logPath.empty()) {
			strcpy(argv[argc++], Cmd::LogPathOption().c_str());
			strcpy(argv[argc++], task.logPath.c_str());
		}

		Cmd::run(argc, argv);
	}

	void Simulator::run(const String &envPath) {
		char argBuf[Cmd::MaxArgNum][Cmd::MaxArgLen];
		char *argv[Cmd::MaxArgNum];
		for (int i = 0; i < Cmd::MaxArgNum; ++i) { argv[i] = argBuf[i]; }
		strcpy(argv[ArgIndex::ExeName], ProgramName().c_str());

		int argc = ArgIndex::ArgStart;

		strcpy(argv[argc++], Cmd::EnvironmentPathOption().c_str());
		strcpy(argv[argc++], envPath.c_str());

		Cmd::run(argc, argv);
	}

	void Simulator::debug() {
		Task task;
		task.instSet = "";
		task.instId = "Y.0.0_seed=0";
		task.randSeed = "1500972793";
		//task.randSeed = to_string(RandSeed::generate());
		task.timeout = "180";
		//task.maxIter = "1000000000";
		task.jobNum = "1";
		task.cfgPath = Env::DefaultCfgPath();
		task.logPath = Env::DefaultLogPath();
		task.runId = "0";

		run(task);
	}

	void Simulator::benchmark(int repeat) {
		Task task;
		task.instSet = "";
		//task.timeout = "180";
		//task.maxIter = "1000000000";
		task.timeout = "3600";
		//task.maxIter = "1000000000";
		task.jobNum = "1";
		task.cfgPath = Env::DefaultCfgPath();
		task.logPath = Env::DefaultLogPath();

		random_device rd;
		mt19937 rgen(rd());
		for (int i = 0; i < repeat; ++i) {
			//shuffle(instList.begin(), instList.end(), rgen);
			for (auto inst = instList.begin(); inst != instList.end(); ++inst) {
				task.instId = *inst;
				task.randSeed = to_string(Random::generateSeed());
				task.runId = to_string(i);
				run(task);
			}
		}
	}

	void Simulator::parallelBenchmark(int repeat) {
		Task task;
		task.instSet = "";
		//task.timeout = "180";
		//task.maxIter = "1000000000";
		task.timeout = "3600";
		//task.maxIter = "1000000000";
		task.jobNum = "1";
		task.cfgPath = Env::DefaultCfgPath();
		task.logPath = Env::DefaultLogPath();

		ThreadPool<> tp(3);

		random_device rd;
		mt19937 rgen(rd());
		for (int i = 0; i < repeat; ++i) {
			//shuffle(instList.begin(), instList.end(), rgen);
			for (auto inst = instList.begin(); inst != instList.end(); ++inst) {
				task.instId = *inst;
				task.randSeed = to_string(Random::generateSeed());
				task.runId = to_string(i);
				tp.push([=]() { run(task); });
				this_thread::sleep_for(1s);
			}
		}
	}


	void Simulator::convertYandZInstance(const String trfPath, const String netPath,const String table) {
		
		ifstream trf(InstanceDir()+table+trfPath + ".trf");
		ifstream net(InstanceDir()+table+netPath + ".net");

		int nodeNum, edgeNum;
		net >> nodeNum >> edgeNum;

		Arr2D<int> edgeIndices(nodeNum, nodeNum, -1);

		Problem::Input input;

		auto &graph(*input.mutable_graph());
		graph.set_nodenum(nodeNum);
		for (int e = 0; e < edgeNum; ++e) {
			int source, target;
			net >> source >> target;

			if (source > target) { swap(source, target); }

			if (edgeIndices.at(source, target) < 0) {
				edgeIndices.at(source, target) = graph.edges().size();
				auto &edge(*graph.add_edges());
				edge.set_source(source);
				edge.set_target(target);
			}
		}
		
		int trafficNum;
		trf >> trafficNum;
		for (auto t = 0; t < trafficNum; ++t) {
			int src, dst;
			trf >> src >> dst;
			auto &traffics(*input.add_traffics());
			traffics.set_src(src);
			traffics.set_dst(dst);
			traffics.set_id(t);
		}

		ostringstream outPath;
		outPath << InstanceDir() <<   trfPath+".json";
		save(outPath.str(), input);
	}


}
