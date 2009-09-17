#ifndef TI_JOB_H
#define TI_JOB_H

#include <string>
#include <vector>

using std::string;
using std::wstring;
using std::vector;

namespace ti
{
	class InstallJob
	{
			public:
			InstallJob(bool isUpdate = false) :
				isUpdate(isUpdate) {}
			string name;
			string version;
			string url;
			bool isUpdate;

			static vector<InstallJob*> ReadJobs(wstring &filename);
	};
}

#endif
