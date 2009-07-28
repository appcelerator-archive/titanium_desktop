#ifndef TI_JOB_H
#define TI_JOB_H

#include <string>
#include <vector>

namespace ti
{
	class InstallJob
	{
			public:
			InstallJob(bool isUpdate = false) :
				isUpdate(isUpdate) {}
			std::string name;
			std::string version;
			std::string url;
			bool isUpdate;

			static std::vector<InstallJob*> ReadJobs(std::string &filename);
	};
}

#endif
