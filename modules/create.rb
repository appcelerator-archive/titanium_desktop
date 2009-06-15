#!/usr/bin/env ruby
#
# script for creating a template module that can
# allow you to get started really fast with basic
# files pre-created
#
#
require 'fileutils'

dir = File.expand_path(File.dirname(__FILE__))

arg = ARGV.first
if arg.nil?
  $stderr.puts "Usage: create.rb <module_name>"
  exit 1
end

name = arg.downcase

if name=~/^ti\./
  s = name.index '.'
  name = name[s+1..-1]
end

module_name = name.capitalize
module_dir_name = "ti.#{module_name}"
header_define = name.upcase
module_lib_name = module_dir_name.gsub '.','_'

FileUtils.mkdir_p module_dir_name unless File.exists?(module_dir_name)

bh = File.open(File.join(module_dir_name,"#{name}_binding.h"),'w')
bh.puts <<-END
/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license.
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef _#{header_define}_BINDING_H_
#define _#{header_define}_BINDING_H_

#include <kroll/kroll.h>

namespace ti
{
	class #{module_name}Binding : public StaticBoundObject
	{
	public:
		#{module_name}Binding(SharedKObject);
	protected:
		virtual ~#{module_name}Binding();
	private:
		SharedKObject global;
	};
}

#endif
END
bh.close


bc = File.open(File.join(module_dir_name,"#{name}_binding.cpp"),'w')
bc.puts <<-END
/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */	
#include <kroll/kroll.h>
#include "#{name}_binding.h"

namespace ti
{
	#{module_name}Binding::#{module_name}Binding(SharedKObject global) : global(global)
	{
	}
	#{module_name}Binding::~#{module_name}Binding()
	{
	}
}
END
bc.close

mh = File.open(File.join(module_dir_name,"#{name}_module.h"),'w')
mh.puts <<-END
/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */

#ifndef TI_#{header_define}_MODULE_H_
#define TI_#{header_define}_MODULE_H_

#include <kroll/kroll.h>

#if defined(OS_OSX) || defined(OS_LINUX)
#define EXPORT __attribute__((visibility("default")))
#define TITANIUM_#{header_define}_API EXPORT
#elif defined(OS_WIN32)
# ifdef TITANIUM_#{header_define}_API_EXPORT
#  define TITANIUM_#{header_define}_API __declspec(dllexport)
# else
#  define TITANIUM_#{header_define}_API __declspec(dllimport)
# endif
# define EXPORT __declspec(dllexport)
#endif

namespace ti 
{
	class TITANIUM_#{header_define}_API #{module_name}Module : public kroll::Module
	{
		KROLL_MODULE_CLASS(#{module_name}Module)
		
	private:
		kroll::SharedKObject binding;
	};

}
#endif
END
mh.close

mc = File.open(File.join(module_dir_name,"#{name}_module.cpp"),'w')
mc.puts <<-END
/**
 * Appcelerator Titanium - licensed under the Apache Public License 2
 * see LICENSE in the root folder for details on the license. 
 * Copyright (c) 2009 Appcelerator, Inc. All Rights Reserved.
 */
#include <kroll/kroll.h>
#include "#{name}_module.h"
#include "#{name}_binding.h"

using namespace kroll;
using namespace ti;

namespace ti
{
	KROLL_MODULE(#{module_name}Module);
	
	void #{module_name}Module::Initialize()
	{
		// load our variables
		this->binding = new #{module_name}Binding(host->GetGlobalObject());

		// set our #{module_dir_name}
		SharedValue value = Value::NewObject(this->binding);
		host->GetGlobalObject()->Set("#{module_name}", value);
	}

	void #{module_name}Module::Stop()
	{
	}
	
}
END
mc.close


sc = File.open(File.join(module_dir_name,"SConscript"),'w')
sc.puts <<-END
#!/usr/bin/env python
import os
Import('build')

env = build.env.Clone();
env.Append(CPPDEFINES = ('TITANIUM_#{header_define}_API_EXPORT', 1))
env.Append(CPPPATH = ['#kroll'])
build.add_thirdparty(env, 'poco')

m = build.add_module('ti.#{name}')
t = env.SharedLibrary(target = m.build_dir + '/ti#{name}module', source = Glob('*.cpp'))
build.mark_build_target(t)
END
sc.close

mf = File.open(File.join(module_dir_name,"manifest"),'w')
mf.puts <<-END
#name: #{module_dir_name}
#version: 0.1
#description: #{module_dir_name}
#os: win32, linux, osx
#depends:
END
mf.close


puts "Created: #{module_dir_name}"
exit 0

