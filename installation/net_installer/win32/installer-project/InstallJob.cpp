#include "InstallJob.h"
#include "api/utils/utils.h"
#include <string>

using KrollUtils::FileUtils;

namespace ti
{
	std::vector<InstallJob*> InstallJob::ReadJobs(std::string &filename)
	{
		std::vector<InstallJob*> jobs;
		
		if (!filename.empty() && FileUtils::IsFile(filename)) {
			std::ifstream file(filename.c_str());
			if (!file.bad() && !file.fail() && !file.eof())
			{
				std::string line;				
				char url[1024];
				char name[1024];
				char version[1024];
	
				while(!std::getline(file, line).eof())
				{
					InstallJob *job = new InstallJob();
					
					int firstComma = line.find(",");
					int secondComma = line.find(",", firstComma+1);
					job->name = line.substr(0, firstComma);
					job->version = line.substr(firstComma+1, secondComma-firstComma-1);
					job->url = line.substr(secondComma+1);
					
					jobs.push_back(job);
				}
			}
		}
		
		return jobs;
	}
}