////////////////////////////////
/// usage : 1.  prepare the input, call the solver through command line interface
///             or class library interface, and then collect output for analysis.
///
/// note :  1.
////////////////////////////////

#ifndef SIMULATOR_H
#define SIMULATOR_H


#include <string>
#include <vector>

#include "../Solver/RWAsolver.h"
#include "../Solver/Problem.h"
#include "../Solver/Utility.h"


namespace szx {

	using Cmd = RWAsolver::Cli;
	using Env = RWAsolver::Environment;

	class Simulator {
#pragma region Type
	public:
		// TODO[szx][4]: make it be able to run with serveral env and cfg.
		struct Task {
			String instSet;
			String instId;
			String randSeed;
			String currentBest;
			String timeout;
			String maxIter;
			String jobNum;
			String cfgPath;
			String logPath;
			String runId;
		};

		struct InstanceTrait {
			Interval<int> nodeNum = Interval<int>(5000, Problem::MaxNodeNum);
			Interval<int> centerNum = Interval<int>(2000, Problem::MaxCenterNum);
		};
#pragma endregion Type

#pragma region Constant
	public:
		static String InstanceDir() { return Env::DefaultInstanceDir(); }
		static String SolutionDir() { return Env::DefaultSolutionDir(); }

		static String ProgramName() { return "Simulator.exe"; }

		enum ArgIndex { ExeName = 0, ArgStart };
#pragma endregion Constant

#pragma region Constructor
	public:
#pragma endregion Constructor

#pragma region Method
	public:
		static void initDefaultEnvironment();

		// invoke solver with arguments explicitly specifying running information.
		void run(const Task &task);
		// invoke solver with single argument of environment file path.
		void run(const String &envPath = Env::DefaultEnvPath());

		// utility for debugging solver with certain arguments.
		void debug();
		// utility for testing all instances.
		void benchmark(int repeat = 1);
		// utility for testing all instances using a thread pool.
		void parallelBenchmark(int repeat);

		void convertYandZInstance(const String trfPath, const String netPath,const String table);
#pragma endregion Method

#pragma region Field
	public:
#pragma endregion Field
	};

}


#endif  // SIMULATOR_H
