#pragma once
#include<vector>
#include<iostream>
#include<sstream>
#include<fstream>
#include <thread>

#include "gurobi_c++.h"
#include"Common.h"
#include"Utility.h"
#include"LogSwitch.h"
#include "Problem.h"
namespace szx {
	class RWAsolver {
#pragma region Type
	public:
		struct Cli {
			static constexpr int MaxArgLen = 256;
			static constexpr int MaxArgNum = 32;

			static String InstancePathOption() { return "-p"; }
			static String SolutionPathOption() { return "-o"; }
			static String RandSeedOption() { return "-s"; }
			static String TimeoutOption() { return "-t"; }
			static String MaxIterOption() { return "-i"; }
			static String JobNumOption() { return "-j"; }
			static String RunIdOption() { return "-rid"; }
			static String EnvironmentPathOption() { return "-env"; }
			static String ConfigPathOption() { return "-cfg"; }
			static String LogPathOption() { return "-log"; }

			static String AuthorNameSwitch() { return "-name"; }
			static String HelpSwitch() { return "-h"; }

			static String AuthorName() { return "szx"; }
			static String HelpInfo() {
				return "Pattern (args can be in any order):\n"
					"  exe (-p path) (-o path) [-s int] [-t seconds] [-name]\n"
					"      [-iter int] [-j int] [-id string] [-h]\n"
					"      [-env path] [-cfg path] [-log path]\n"
					"Switches:\n"
					"  -name  return the identifier of the authors.\n"
					"  -h     print help information.\n"
					"Options:\n"
					"  -p     input instance file path.\n"
					"  -o     output solution file path.\n"
					"  -s     rand seed for the solver.\n"
					"  -t     max running time of the solver.\n"
					"  -i     max iteration of the solver.\n"
					"  -j     max number of working solvers at the same time.\n"
					"  -rid   distinguish different runs in log file and output.\n"
					"  -env   environment file path.\n"
					"  -cfg   configuration file path.\n"
					"  -log   activate logging and specify log file path.\n"
					"Note:\n"
					"  0. in pattern, () is non-optional group, [] is optional group\n"
					"     when -env option is not given.\n"
					"  1. an environment file contains information of all options.\n"
					"     explicit options get higher priority than this.\n"
					"  2. reaching either timeout or iter will stop the solver.\n"
					"     if you specify neither of them, the solver will be running\n"
					"     for a long time. so you should set at least one of them.\n"
					"  3. the solver will still try to generate an initial solution\n"
					"     even if the timeout or max iteration is 0. but the solution\n"
					"     is not guaranteed to be feasible.\n";
			}

			// a dummy main function.
			static int run(int argc, char *argv[]);
		};
		struct Configuration {
			enum Algorithm { Greedy, TreeSearch, DynamicProgramming, LocalSearch, Genetic, MathematicallProgramming };


			Configuration() {}


			String toBriefStr() const {
				String threadNum(std::to_string(threadNumPerWorker));
				std::ostringstream oss;
				oss << "alg=" << alg
					<< ";job=" << threadNum;
				return oss.str();
			}

			
			Algorithm alg = Configuration::Algorithm::Greedy; // OPTIMIZE[szx][3]: make it a list to specify a series of algorithms to be used by each threads in sequence.
			int threadNumPerWorker = (std::min)(1, static_cast<int>(std::thread::hardware_concurrency()));
		};

		// describe the requirements to the input and output data interface.
		struct Environment {
			static constexpr int DefaultTimeout = (1 << 30);
			static constexpr int DefaultMaxIter = (1 << 30);
			static constexpr int DefaultJobNum = 0;
			// preserved time for IO in the total given time.
			static constexpr int SaveSolutionTimeInMillisecond = 1000;

			static constexpr Duration RapidModeTimeoutThreshold = 600 * static_cast<Duration>(Timer::MillisecondsPerSecond);

			static String DefaultInstanceDir() { return "Instance/"; }
			static String DefaultSolutionDir() { return "Solution/"; }
			static String DefaultVisualizationDir() { return "Visualization/"; }
			static String DefaultEnvPath() { return "env.csv"; }
			static String DefaultCfgPath() { return "cfg.csv"; }
			static String DefaultLogPath() { return "log.csv"; }

			Environment(const String &instancePath, const String &solutionPath,
				int randomSeed = Random::generateSeed(), double timeoutInSecond = DefaultTimeout,
				Iteration maxIteration = DefaultMaxIter, int jobNumber = DefaultJobNum, String runId = "",
				const String &cfgFilePath = DefaultCfgPath(), const String &logFilePath = DefaultLogPath())
				: instPath(instancePath), slnPath(solutionPath), randSeed(randomSeed),
				msTimeout(static_cast<Duration>(timeoutInSecond * Timer::MillisecondsPerSecond)), maxIter(maxIteration),
				jobNum(jobNumber), rid(runId), cfgPath(cfgFilePath), logPath(logFilePath), localTime(Timer::getTightLocalTime()) {}
			Environment() : Environment("", "") {}

			void load(const Map<String, char*> &optionMap);
			void load(const String &filePath);
			void loadWithoutCalibrate(const String &filePath);
			void save(const String &filePath) const;

			void calibrate(); // adjust job number and timeout to fit the platform.

			String solutionPathWithTime() const { return slnPath + "." + localTime; }

			String visualizPath() const { return DefaultVisualizationDir() + friendlyInstName() + "." + localTime + ".html"; }
			template<typename T>
			String visualizPath(const T &msg) const { return DefaultVisualizationDir() + friendlyInstName() + "." + localTime + "." + std::to_string(msg) + ".html"; }
			String friendlyInstName() const { // friendly to file system (without special char).
				auto pos = instPath.find_last_of('/');
				return (pos == String::npos) ? instPath : instPath.substr(pos + 1);
			}
			String friendlyLocalTime() const { // friendly to human.
				return localTime.substr(0, 4) + "-" + localTime.substr(4, 2) + "-" + localTime.substr(6, 2)
					+ "_" + localTime.substr(8, 2) + ":" + localTime.substr(10, 2) + ":" + localTime.substr(12, 2);
			}

			// essential information.
			String instPath;
			String slnPath;
			int randSeed;
			Duration msTimeout;

			// optional information. highly recommended to set in benchmark.
			Iteration maxIter;
			int jobNum; // number of solvers working at the same time.
			String rid; // the id of each run.
			String cfgPath;
			String logPath;

			// auto-generated data.
			String localTime;
		};
		struct Solution : public Problem::Output { // cutting patterns.
			Solution(RWAsolver *pSolver = nullptr) : solver(pSolver) {}

			RWAsolver *solver;
		};

		struct Topo {
		public:
			List<List<ID>> linkNodes; // linkNodes[i]为结点i连接的所有的结点集合
			List<List<ID>> nextID; // nextID[i][j]为i到j的最短路径中i的下一个结点
			List<List<ID>> adjLinkMat; // adjLinkMat[i][j]为从节点i到结点j的链路ID，ID从1开始编号
			Arr2D<Length> adjMat;
			
			Topo() {}
		};
		struct Traffic {
		public:
			ID id;
			ID src;
			ID dst;
			List<ID> nodes; // 业务的路径
			List<ID> shorestNodes; // 业务在不考虑波长分配时的最短路径
			Wave wave; // 业务使用的波长
			Traffic(int id,int src, int dst) :id(id),src(src), dst(dst) {}
		};
		struct WaveGroup {
		public:
			List<Traffic *> tras;
			Arr2D<Length> adjMat; // 该颜色盒子的网络拓扑
			WaveGroup(Arr2D<Length> &originalAdjMat) {
				int nodeNum = originalAdjMat.size1();
				adjMat.init(nodeNum, nodeNum);
				for (auto i = 0; i < nodeNum; ++i) {
					for (auto j = 0; j < nodeNum; ++j) {
						adjMat.at(i, j) = originalAdjMat.at(i, j);
					}
				}
				
			}
			void addTra(Traffic *tra) {
				tras.push_back(tra);
				for (auto i = 0; i < tra->nodes.size() -1; ++i) {
					ID node1 = tra->nodes[i], node2 = tra->nodes[i + 1];
					adjMat[node1][node2] = MaxDistance;
					adjMat[node2][node1] = MaxDistance;
				}
			}
			void deleteTra(Traffic *tra) { // TODO[fy][0]: 这里数据结构之后考虑使用双向表
				for (auto iter = tras.begin(); iter != tras.end(); ++iter) {
					if (*iter == tra) {
						tras.erase(iter);
						break;
					}
				}
				for (auto i = 0; i < tra->nodes.size()-1; ++i) { // 这里还有问题，因为可能该颜色盒子本身业务路径是有重叠的，不能直接恢复链路!!!
					ID node1 = tra->nodes[i], node2 = tra->nodes[i + 1];
					adjMat[node1][node2] = 1;
					adjMat[node2][node1] = 1;
				}
			}
		};
		struct Output {
		public:
			void save(List<Traffic> &traffics, int cost) {
				std::ofstream ofs;
				ofs.open("result.txt", std::ios::out);
				if (!ofs.is_open()) {
					std::cout << "[Fatal] Invalid answer path!" << std::endl;
				}
				else {
					ofs << cost << std::endl;
					for (auto i = 0; i < traffics.size(); ++i) {
						ofs << "Traffic" << i << ":  ";
						for (auto j = 0; j < traffics[i].nodes.size(); ++j) {
							ofs << traffics[i].nodes[j] << " ";
						}
						ofs << std::endl;
					}
				}
			}
		};




#pragma endregion Type


#pragma region Constant
	public:
		
#pragma endregion Constant

#pragma region Constructor
	public:
		RWAsolver(const Problem::Input &inputData, const Environment &environment, const Configuration &config):input(inputData), env(environment), cfg(config), rand(environment.randSeed),
			timer(std::chrono::milliseconds(environment.msTimeout)), iteration(1) {};

#pragma endregion Constructor

#pragma region Method
	public:
		~RWAsolver();
		bool solve();
		bool check(Length &obj) const;
		void record() const; // save running log.

		bool optimize(Solution &sln, ID workerId = 0);


		
#pragma endregion Method

#pragma region Field
	public:
		Environment env;
		Configuration cfg;
		Problem::Input input;
		Problem::Output output;
		Random rand; // all random number in Solver must be generated by this.
		Timer timer; // the solve() should return before it is timeout.
		Iteration iteration;

#pragma endregion Field
	}; // RWAsolver
}



