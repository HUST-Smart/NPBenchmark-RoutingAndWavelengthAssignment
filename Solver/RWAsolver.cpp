#include "RWAsolver.h"

using namespace std;
namespace szx {
#pragma region RWAsolver::Cli
	int RWAsolver::Cli::run(int argc, char * argv[]) {
		Log(LogSwitch::Szx::Cli) << "parse command line arguments." << std::endl;
		Set<String> switchSet;
		Map<String, char*> optionMap({ // use string as key to compare string contents instead of pointers.
			{ InstancePathOption(), nullptr },
			{ SolutionPathOption(), nullptr },
			{ RandSeedOption(), nullptr },
			{ TimeoutOption(), nullptr },
			{ MaxIterOption(), nullptr },
			{ JobNumOption(), nullptr },
			{ RunIdOption(), nullptr },
			{ EnvironmentPathOption(), nullptr },
			{ ConfigPathOption(), nullptr },
			{ LogPathOption(), nullptr }
			});

		for (int i = 1; i < argc; ++i) { // skip executable name.
			auto mapIter = optionMap.find(argv[i]);
			if (mapIter != optionMap.end()) { // option argument.
				mapIter->second = argv[++i];
			}
			else { // switch argument.
				switchSet.insert(argv[i]);
			}
		}

		Log(LogSwitch::Szx::Cli) << "execute commands." << endl;
		if (switchSet.find(HelpSwitch()) != switchSet.end()) {
			cout << HelpInfo() << endl;
		}

		if (switchSet.find(AuthorNameSwitch()) != switchSet.end()) {
			cout << AuthorName() << endl;
		}

		RWAsolver::Environment env;
		env.load(optionMap);
		if (env.instPath.empty() || env.slnPath.empty()) { return -1; }

		RWAsolver::Configuration cfg;

		Log(LogSwitch::Szx::Input) << "load instance " << env.instPath << " (seed=" << env.randSeed << ")." << endl;
		Problem::Input input;
		if (!input.load(env.instPath)) { 
			cout << "error";
			return -1; 
		}

		RWAsolver solver(input, env, cfg);
		solver.solve();

		pb::Submission submission;
		submission.set_thread(to_string(env.jobNum));
		submission.set_instance(env.friendlyInstName());
		submission.set_duration(to_string(solver.timer.elapsedSeconds()) + "s");

		solver.output.save(env.slnPath, submission);
#if SZX_DEBUG
		solver.output.save(env.solutionPathWithTime(), submission);
		solver.record();
#endif // SZX_DEBUG

		return 0;
	}
#pragma endregion RWAsolver::Cli
#pragma region RWAsolver::Environment
	void RWAsolver::Environment::load(const Map<String, char*> &optionMap) {
		char *str;

		str = optionMap.at(Cli::EnvironmentPathOption());
		if (str != nullptr) { loadWithoutCalibrate(str); }

		str = optionMap.at(Cli::InstancePathOption());
		if (str != nullptr) { instPath = str; }

		str = optionMap.at(Cli::SolutionPathOption());
		if (str != nullptr) { slnPath = str; }

		str = optionMap.at(Cli::RandSeedOption());
		if (str != nullptr) { randSeed = atoi(str); }

		str = optionMap.at(Cli::TimeoutOption());
		if (str != nullptr) { msTimeout = static_cast<Duration>(atof(str) * Timer::MillisecondsPerSecond); }

		str = optionMap.at(Cli::MaxIterOption());
		if (str != nullptr) { maxIter = atoi(str); }

		str = optionMap.at(Cli::JobNumOption());
		if (str != nullptr) { jobNum = atoi(str); }

		str = optionMap.at(Cli::RunIdOption());
		if (str != nullptr) { rid = str; }

		str = optionMap.at(Cli::ConfigPathOption());
		if (str != nullptr) { cfgPath = str; }

		str = optionMap.at(Cli::LogPathOption());
		if (str != nullptr) { logPath = str; }

		calibrate();
	}

	void RWAsolver::Environment::load(const String &filePath) {
		loadWithoutCalibrate(filePath);
		calibrate();
	}

	void RWAsolver::Environment::loadWithoutCalibrate(const String &filePath) {
		// EXTEND[szx][8]: load environment from file.
		// EXTEND[szx][8]: check file existence first.
	}

	void RWAsolver::Environment::save(const String &filePath) const {
		// EXTEND[szx][8]: save environment to file.
	}
	void RWAsolver::Environment::calibrate() {
		// adjust thread number.
		int threadNum = thread::hardware_concurrency();
		if ((jobNum <= 0) || (jobNum > threadNum)) { jobNum = threadNum; }

		// adjust timeout.
		msTimeout -= Environment::SaveSolutionTimeInMillisecond;
	}
#pragma endregion RWAsolver::Environment
	 List<ID> getPath(List<List<ID>> &nextID, ID src, ID dst) {
		List<ID> path;
		ID temp = src;
		path.push_back(src);
		while (temp != dst) {
			temp = nextID[temp][dst];
			path.push_back(temp);
		}
		return path;
	}

	void printPath(List<ID> &path) {
		for (auto i = 0; i < path.size(); ++i) {
			std::cout << path[i] << "  ";
		}
		std::cout << "\n";
	}

//	RWAsolver::RWAsolver()
//	{
//		String InstancePath = ".//dataset//table3//";
//		String net_file = "test.net";
//		String trf_file = "test.trf";
//		net_file = InstancePath + net_file;
//		trf_file = InstancePath + trf_file;
//		readInstance(net_file, trf_file);
//
//		optimize();
//
//
///*		List<Traffic *> tempTras;
//		for (auto i = 0; i < 2; ++i) {
//			tempTras.push_back(&traffics[i]);
//		}
//		
//		Model model(tempTras,nodeNum,topo);
//		traNumShouldRemove = model.traNumShouldRemove;
//
//		
//		output.save(traffics, traNumShouldRemove);*/
//	}

	RWAsolver::~RWAsolver()
	{

	}

	bool RWAsolver::solve()
	{
		//init();

		int workerNum = (max)(1, env.jobNum / cfg.threadNumPerWorker);
		cfg.threadNumPerWorker = env.jobNum / workerNum;
		List<Solution> solutions(workerNum, Solution(this));
		List<bool> success(workerNum);

		Log(LogSwitch::Szx::Framework) << "launch " << workerNum << " workers." << endl;
		List<thread> threadList;
		threadList.reserve(workerNum);
		for (int i = 0; i < workerNum; ++i) {
			threadList.emplace_back([&, i]() { success[i] = optimize(solutions[i], i); });
		}
		for (int i = 0; i < workerNum; ++i) { threadList.at(i).join(); }

		Log(LogSwitch::Szx::Framework) << "collect best result among all workers." << endl;
		int bestIndex = -1;
		Length bestValue = Problem::MaxDistance;
		for (int i = 0; i < workerNum; ++i) {
			if (!success[i]) { continue; }
			Log(LogSwitch::Szx::Framework) << "worker " << i << " got " << solutions[i].waveLengthNum << endl;
			if (solutions[i].waveLengthNum >= bestValue) { continue; }
			bestIndex = i;
			bestValue = solutions[i].waveLengthNum;
		}

		env.rid = to_string(bestIndex);
		if (bestIndex < 0) { return false; }
		output = solutions[bestIndex];
		return true;
	}

	bool RWAsolver::check(Length & checkerObj) const
	{
#if SZX_DEBUG
		enum CheckerFlag {
			IoError = 0x0,
			FormatError = 0x1,
			TooManyCentersError = 0x2
		};

		checkerObj = System::exec("Checker.exe " + env.instPath + " " + env.solutionPathWithTime());
		if (checkerObj > 0) { return true; }
		checkerObj = ~checkerObj;
		if (checkerObj == CheckerFlag::IoError) { Log(LogSwitch::Checker) << "IoError." << endl; }
		if (checkerObj & CheckerFlag::FormatError) { Log(LogSwitch::Checker) << "FormatError." << endl; }
		if (checkerObj & CheckerFlag::TooManyCentersError) { Log(LogSwitch::Checker) << "TooManyCentersError." << endl; }
		return false;
#else
		checkerObj = 0;
		return true;
#endif // SZX_DEBUG
	}

	void RWAsolver::record() const
	{
#if SZX_DEBUG
		int generation = 0;

		ostringstream log;

		System::MemoryUsage mu = System::peakMemoryUsage();

		Length obj = output.waveLengthNum;
		Length checkerObj = -1;
		bool feasible = 1;
		//feasible = check(checkerObj);

		// record basic information.
		log << env.friendlyLocalTime() << ","
			<< env.rid << ","
			<< env.instPath << ","
			<< feasible << "," << (obj - checkerObj) << ",";
		log << obj << ",";
		log << timer.elapsedSeconds() << ","
			<< mu.physicalMemory << "," << mu.virtualMemory << ","
			<< env.randSeed << ","
			<< cfg.toBriefStr() << ","
			<< generation << "," << iteration << ",";

		for (auto t = output.traout().begin(); t != output.traout().end(); ++t) {
			log << "id: " << (*t).id() << " ";
			log << "w:"<<(*t).wave()<<"  p: ";
			for (auto o = (*t).path().begin(); o != (*t).path().end(); ++o) {
				log << *o << " ";
			}
			log << ";";
		}
		log << endl;

		// append all text atomically.
		static mutex logFileMutex;
		lock_guard<mutex> logFileGuard(logFileMutex);

		ofstream logFile(env.logPath, ios::app);
		logFile.seekp(0, ios::end);
		if (logFile.tellp() <= 0) {
			logFile << "Time,ID,Instance,Feasible,ObjMatch,waveLengthNum,Duration,PhysMem,VirtMem,RandSeed,Config,Generation,Iteration,Solution" << endl;
		}
		logFile << log.str();
		logFile.close();
#endif // SZX_DEBUG
	}

	bool RWAsolver::optimize(Solution & sln, ID workerId)
	{
		// TODO[optimize]: 添加自己的算法
		// 输入和输出接口请查看 /lib/RWA.proto

		/// 使用样例
		for (auto i = input.traffics().begin(); i != input.traffics().end() ; ++i) {
			ID src = (*i).src();
			ID dst = (*i).dst();
		}
		sln.waveLengthNum = 1; // 保存结果，总使用波长数
		for (auto i = input.traffics().begin(); i != input.traffics().end(); ++i) {// 计算所有业务的路径及波长
			ID src = (*i).src();
			ID dst = (*i).dst(); 
			ID id = (*i).id();
			auto &traout(*sln.add_traout());
			traout.set_wave(1);
			traout.set_id(id);

			traout.add_path(src);
			traout.add_path(dst);
		}
		
		

		return true;
	}

}
