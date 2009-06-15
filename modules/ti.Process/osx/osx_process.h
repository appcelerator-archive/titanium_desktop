/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _OSX_PROCESS_H_
#define _OSX_PROCESS_H_
#include <kroll/kroll.h>
#include <Cocoa/Cocoa.h>
#include "../process_binding.h"
#include "osx_pipe.h"

@interface TiOSXProcess : NSObject
{
	NSTask *task;
	Host *host;
	SharedKMethod *onread;
	SharedKMethod *onexit;
	ti::OSXPipe *input;
	ti::OSXPipe *output;
	ti::OSXPipe *error;
	SharedKObject *shared_input;
	SharedKObject *shared_output;
	SharedKObject *shared_error;
	KObject *bound;
}
-(id)initWithPath:(NSString*)cmd args:(NSArray*)args host:(Host*)host bound:(KObject*)bo;
-(NSTask*)task;
-(void)start;
-(void)stop;
-(void)setRead: (SharedKMethod*)method;
-(void)setExit: (SharedKMethod*)method;
-(void)getOutData: (NSNotification *)aNotification;
-(void)getErrData: (NSNotification *)aNotification;
-(void)terminated: (NSNotification *)aNotification;
-(ti::OSXPipe*) output;
-(ti::OSXPipe*) error;
@end

namespace ti
{
	class OSXProcess : public StaticBoundObject
	{
	public:
		OSXProcess(ProcessBinding* parent, std::string& cmd, std::vector<std::string>& args);
		virtual ~OSXProcess();
	private:
		Host *host;
		TiOSXProcess *process;
	public:

	protected:
		void Bound(const char *name, SharedValue value);
		void Terminate(const ValueList& args, SharedValue result);
	};
}

#endif
