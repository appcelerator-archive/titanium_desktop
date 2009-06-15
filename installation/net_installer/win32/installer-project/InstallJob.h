#ifndef TI_JOB_H
#define TI_JOB_H

#include <string>
#include <vector>

namespace ti
{
	class InstallJob {
		public:
			static std::vector<InstallJob*> ReadJobs(std::string &filename);
			
			std::string name, version, url;
			bool isUpdate;
			
			InstallJob(bool isUpdate = false) : isUpdate(isUpdate) {}
	};
}

#endif