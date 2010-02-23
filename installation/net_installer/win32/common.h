/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009-2010 Appcelerator, Inc. All Rights Reserved.
 */
void ShowError(const std::string& msg);
void ShowError(const std::wstring& wmsg);
void ShutdownNetConnection();
bool DownloadDependency(SharedApplication app, SharedDependency dependency);
bool InstallDependency(SharedApplication app, SharedDependency dependency);
