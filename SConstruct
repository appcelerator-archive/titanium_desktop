#!/usr/bin/env python

# this will ensure that you're using the right version of scons
EnsureSConsVersion(1,2,0)
# this will ensure that you're using the right version of python
EnsurePythonVersion(2,5)

# common SConscripts

import os, re, sys, inspect, os.path as path
from sets import Set
import subprocess, distutils.dir_util as dir_util
sys.path.append(path.join(path.abspath('.'), 'build'))
import titanium_version

from kroll import BuildConfig
build = BuildConfig(
	PRODUCT_VERSION = titanium_version.version,
	INSTALL_PREFIX = '/usr/local',
	PRODUCT_NAME = 'Titanium',
	GLOBAL_NS_VARNAME = 'Titanium',
	CONFIG_FILENAME = 'tiapp.xml',
	BUILD_DIR = path.abspath('build'),
	THIRD_PARTY_DIR = path.join(path.abspath('kroll'), 'thirdparty'),
	DISTRIBUTION_URL = 'api.appcelerator.net',
	CRASH_REPORT_URL = 'api.appcelerator.net/p/v1/app-crash-report'
)
build.set_kroll_source_dir(path.abspath('kroll'))

build.titanium_source_dir = path.abspath('.')
build.titanium_support_dir = path.join(build.titanium_source_dir, 'support', build.os)

# This should only be used for accessing various
# scripts in the kroll build directory. All resources
# should instead be built to build.dir
build.kroll_build_dir = path.join(build.kroll_source_dir, 'build')

build.env.Append(CPPPATH=[
	build.titanium_source_dir,
	build.kroll_source_dir,
	build.kroll_include_dir
])

# debug build flags
debug = ARGUMENTS.get('debug', 0)
if debug:
	build.env.Append(CPPDEFINES = ('DEBUG', 1))
	if build.is_win32():
		build.env.Append(CCFLAGS=['/Z7'])  # max debug
		build.env.Append(CPPDEFINES=('WIN32_CONSOLE', 1))
	else:
		build.env.Append(CPPFLAGS=['-g'])  # debug
else:
	build.env.Append(CPPDEFINES = ('NDEBUG', 1 ))
	if not build.is_win32():
		build.env.Append(CPPFLAGS = ['-O9']) # max optimizations
if build.is_win32():
	build.env.Append(CCFLAGS=['/EHsc', '/GR', '/MD'])
	build.env.Append(LINKFLAGS=['/DEBUG', '/PDB:${TARGET}.pdb'])

Export('build', 'debug')
targets = COMMAND_LINE_TARGETS
clean = 'clean' in targets or ARGUMENTS.get('clean', 0)
qclean = 'qclean' in targets or ARGUMENTS.get('qclean', 0)
build.nopackage = ARGUMENTS.get('nopackage', 0)

if clean or qclean:
	print "Obliterating your build directory: %s" % build.dir
	if path.exists(build.dir):
		dir_util.remove_tree(build.dir)
	if not qclean: os.system('scons -c')
	Exit(0)

# forcing a crash to test crash detection
if ARGUMENTS.get('test_crash', 0):
	build.env.Append(CPPDEFINES = ('TEST_CRASH_DETECTION', 1))

## Kroll *must not be required* for installation
SConscript('kroll/SConscript.thirdparty', exports='debug')
SConscript('installation/SConscript')

# After Kroll builds, the environment will  link 
# against libkroll, so anything that should not be
# linked against libkroll should be above this point.
SConscript('kroll/SConscript', exports='debug')
SConscript('modules/SConscript')
SConscript('SConscript.dist')
SConscript('SConscript.docs')
SConscript('SConscript.test')

run = ARGUMENTS.get('run', 0)
run_with = ARGUMENTS.get('run_with', 0)

Export('run','run_with')
SConscript('apps/SConscript')
