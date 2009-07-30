#include "install_job.h"
#include "api/utils/utils.h"
#include <string>
using KrollUtils::FileUtils;

namespace ti
{
	std::vector<InstallJob*> InstallJob::ReadJobs(std::string &filename)
	{
		std::vector<InstallJob*> jobs;

		if (filename.empty() || !FileUtils::IsFile(filename))
			return jobs;
		
		std::ifstream file(filename.c_str());
		if (file.bad() || file.fail())
			return jobs;

		std::string line;				
		while (!std::getline(file, line).eof())
		{
			int firstComma = line.find(",");
			int secondComma = line.find(",", firstComma + 1);

			InstallJob *job = new InstallJob();
			job->name = line.substr(0, firstComma);
			job->version = line.substr(firstComma + 1, secondComma - firstComma - 1);
			job->url = line.substr(secondComma + 1);
			jobs.push_back(job);
		}

		file.close();
		return jobs;
	}
}
