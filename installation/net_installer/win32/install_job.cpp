#include "install_job.h"
#include "api/utils/utils.h"
#include <sstream>
using KrollUtils::FileUtils;

extern void ShowError(string msg);

namespace ti
{
	vector<InstallJob*> InstallJob::ReadJobs(wstring &filename)
	{
		vector<InstallJob*> jobs;
		if (filename.empty() || !FileUtils::IsWideFile(filename))
		{
			ShowError(string("Installation jobs file wasn't found at path: ") + KrollUtils::WideToUTF8(filename));
			return jobs;
		}
		
		std::string contents = FileUtils::ReadWideFile(filename);
		std::istringstream jobContents(contents);
		string line;			
		while (std::getline(jobContents, line))
		{
			std::cout << line << std::endl;
			
			int firstComma = line.find(",");
			int secondComma = line.find(",", firstComma + 1);

			InstallJob *job = new InstallJob();
			job->name = line.substr(0, firstComma);
			job->version = line.substr(firstComma + 1, secondComma - firstComma - 1);
			job->url = line.substr(secondComma + 1);
			jobs.push_back(job);
		}
		
		return jobs;
	}
}
