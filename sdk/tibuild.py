#!/usr/bin/env python
#
# This script is the property of Appcelerator, Inc. and
# is Confidential and Proprietary. All Rights Reserved.
# Redistribution without expression written permission
# is not allowed.
#
# Titanium application packaging script
#
# Original author: Jeff Haynie 04/02/09
#
#
from optparse import OptionParser
from desktop_builder import DesktopBuilder
from desktop_packager import DesktopPackager
import sys, os.path, platform, re, subprocess, signal
from xml.etree.ElementTree import ElementTree

VERSION = '0.1'
PLATFORMS = ['win32','osx','linux']
cwd = os.path.abspath(os.path.dirname(sys._getframe(0).f_code.co_filename))

def log(options,msg):
	if options.verbose:
		print msg
		sys.stdout.flush()

def get_from_tiapp(tiapp,name,defv):
	el = tiapp.find(name)
	if el is None or el is -1:
		return defv
	return el.text
	#f = os.path.join(appdir,'tiapp.xml')
	#if not os.path.exists(f):
	#	print "Couldn't find tiapp.xml at: %s" % appdir
	#	sys.exit(1)
	#xml = open(f).read()
	#m = re.search('<%s>(.*?)</%s>' % (name,name),xml)
	#if m==None: return defv
	#return str(m.group(1)).strip()

def get_icon_from_tiapp(tiapp):
	return get_from_tiapp(tiapp,'icon','default_app_logo.png')

def get_dmg_background_from_tiapp(tiapp):
	return get_from_tiapp(tiapp,'dmg_background',None)
	
def get_version_from_tiapp(tiapp):
	return get_from_tiapp(tiapp,'version','1.0')
			
def examine_manifest(appdir):
	f = os.path.join(appdir,'manifest')
	if not os.path.exists(f):
		print "Couldn't find manifest at: %s" % appdir
		sys.exit(1)
	lines = open(f).readlines()
	manifest = {}
	manifest['modules']={}
	for line in lines:
		if len(line)>0:
			tok = line.strip().split(':')
			if line[0:1] == '#':
				manifest[tok[0][1:].strip()]=tok[1].strip()
			else:
				manifest['modules'][tok[0].strip()]=tok[1].strip()
	return manifest

def examine_module_manifest(moduledir):
	f = os.path.join(moduledir, 'manifest')
	manifest = {}
	if os.path.exists(f):
		for line in open(f).readlines():
			tok = line.strip().split(":")
			manifest[tok[0].strip()] = tok[1].strip()
	return manifest

def find_titanium_base():
	p = get_platform()
	f = None
	
	if 'osx' in p:
		f = '/Library/Application Support/Titanium'
		if not os.path.exists(f):
			f = '~/Library/Application Support/Titanium'
	elif 'win32' in p:
		f = 'C:/ProgramData/Titanium'
		if not os.path.exists(f):
			pass
		pass
	elif 'linux' in p:
		f = os.path.expanduser('~/.titanium')
		if not os.path.exists(f):
			f = '/opt/titanium'
	return f	
	
def get_platform():
	if 'Darwin' in platform.platform():
		return 'osx'
	elif 'Windows' in platform.platform():
		return 'win32'
	elif 'Linux' in platform.platform():
		return 'linux'

def desktop_setup(options,appdir):
	# now resolve all the modules and the runtime by version
	depends = options.manifest['modules']
	
	# resolver for module paths
	module_paths = []

	runtime = depends['runtime']
	if os.path.exists(os.path.join(options.source,'runtime',options.platform,runtime)):
		options.runtime_dir = os.path.join(options.source,'runtime',options.platform,runtime)
	else:
		print("Couldn't determine your source distribution for packaging runtime version %s. Please specify." % runtime)
		sys.exit(1)
		
	options.runtime = runtime	

	for name in depends:
		# these are special
		if name == 'runtime' or name == 'sdk' or name == 'mobilesdk':
			continue
		version = depends[name]
		entry = {'name':name,'version':version}
		if os.path.exists(os.path.join(appdir,'modules',name,version)):
			entry['path']=os.path.join(appdir,'modules',name,version)
			entry['manifest'] = examine_module_manifest(entry['path'])
			module_paths.append(entry)
		else:
			# if we're not network, we must find it to be bundled
			# otherwise we must fail
			if options.type != 'network':
				if os.path.exists(os.path.join(options.source,'modules',options.platform,name,version)):
					entry['path']=os.path.join(options.source,'modules',options.platform,name,version)
					entry['manifest'] = examine_module_manifest(entry['path'])
					module_paths.append(entry)
				else:
					print("Couldn't find required module: %s with version: %s" %(name,version))
					sys.exit(1)

	# map in the module paths	
	options.module_paths = module_paths
	
	# assign appdir
	options.appdir = appdir
	
	# assign icon
	options.icon = get_icon_from_tiapp(options.tiapp)
	
	# get the DMG background (OSX only)
	if options.platform == 'osx':
		options.dmg_background = get_dmg_background_from_tiapp(options.tiapp)
	
	# convert this option
	if options.package == 'false' or options.package == 'no':
		options.package = False

	# try and find the assets directory
	if options.assets_dir == None:
		options.assets_dir = os.path.abspath(os.path.dirname(sys._getframe(0).f_code.co_filename))

	if not os.path.exists(options.assets_dir):
		print("Couldn't find assets directory at %s" % options.assets_dir)
		sys.exit(1)

	return DesktopBuilder(options,log)

def run(options):
	
	p = None
	running = True
	
	def handler(signum, frame):
		print "signal caught: %d" % signum
		if not p == None:
			if get_platform() != 'win32':
				print "kill app with pid %d" % p.pid
				os.system("kill -9 %d" % p.pid)
		running = False	
	
	if get_platform() != 'win32':
		signal.signal(signal.SIGHUP, handler)
		signal.signal(signal.SIGQUIT, handler)
		
	signal.signal(signal.SIGINT, handler)
	signal.signal(signal.SIGABRT, handler)
	signal.signal(signal.SIGTERM, handler)
	
	try:
		# this seems to be necessary in some Windows systems
		# could be COM / manifest / CRT related?
		dir = os.path.dirname(options.executable)
		os.chdir(dir)

		p = subprocess.Popen([options.executable,'--debug'])
		os.waitpid(p.pid,0)
	except OSError:
		handler(3,None)
		
				
def main(options,appdir):

	if options.platform == None:
		options.platform = get_platform()
		
	if not options.platform in PLATFORMS:
		print "Error: unsupported/unknown platform: %s" % options.platform
		print "Must be one of: %s" % str(PLATFORMS)
		sys.exit(1)

	log(options,'Packaging for target: %s' % options.platform)
	
	if options.destination == None:
		options.destination = os.path.dirname(sys.argv[0])

	if options.source == None:
		options.source = find_titanium_base()
		if options.source == None:
			print "no source directory specified for distribution files and couldn't find one"
			sys.exit(1)

	if not os.path.exists(options.source):
		print "no source directory found at %s" % options.source
		sys.exit(1)
	
	if not os.path.exists(options.destination):
		print('Invalid destination directory: %s' % options.destination)
		sys.exit(1)
	
	# tiapp ElementTree
	options.tiapp = ElementTree()
	if os.path.exists(os.path.join(appdir, 'tiapp.xml')):
		options.tiapp.parse(os.path.join(appdir, 'tiapp.xml'))
	
	# read the manifest
	options.manifest = examine_manifest(appdir)

	# get the version from the tiapp.xml
	options.version = get_version_from_tiapp(options.tiapp)
	
	# run the builders
	builder = desktop_setup(options,appdir)
	
	# allow post-build scripts
	if os.path.exists("post_builder.py"):
		exec('from post_builder import PostBuilder')
		eval('PostBuilder(builder)')
	
	# run the packagers 
	if not options.run and options.package:
		DesktopPackager(builder)
			
	log(options,"Packaging complete, location: %s"%os.path.abspath(options.destination))

	
	if options.run:
		run(options)

	sys.exit(0)
		
def dequote(s):
	if s[0:1] == '"':
		return s[1:-1]
	return s
	
if __name__ == '__main__':
	parser = OptionParser(usage="%prog [options] appdir", version="%prog " + VERSION)
	parser.add_option("-d","--dest",dest="destination",help="destination folder for output",metavar="FILE")
	parser.add_option("-s","--src",dest="source",help="source folder which contains dist files",metavar="FILE")
	parser.add_option("-v","--verbose",action="store_true",dest="verbose",default=False,help="turn on verbose logging")
	parser.add_option("-o","--os",dest="platform",default=None,help="platform if different than %s" % get_platform())
	parser.add_option("-t","--type",dest="type",default="network",help="package type: network or bundle")
	parser.add_option("-a","--assets",dest="assets_dir",default=None,help="location of platform assets",metavar="FILE")
	parser.add_option("-l","--license",dest="license_file",default=None,help="location of application license",metavar="FILE")
	parser.add_option("-n","--noinstall",action="store_true",dest="no_install",default=False,help="don't include installer dialog in packaged app")
	parser.add_option("-r","--run",action="store_true",dest="run",default=False,help="run the packaged app after building")
	parser.add_option("-p","--package",dest="package",default=True,help="build the installation package")
	parser.add_option("-i","--ignore",dest="ignore_patterns",default="",help="patterns to ignore when packaging, seperated by comma (default: .git,.svn,.gitignore,.cvsignore)")
	(options, args) = parser.parse_args()
	options.packager = False
	if len(args) == 0:
		parser.print_help()
		print 
		print "An example of running this command:"
		print
		print " Build and then run in the current directory from a project in ~/tmp/myproject"
		print "	> tibuild.py -d . -s /Library/Application\ Support/Titanium -a /Library/Application\ Support/Titanium/sdk/osx/0.4.0 -r ~/tmp/myproject"
		print
		sys.exit(1)
	appdir = os.path.abspath(os.path.expanduser(dequote(args[0])))
	if appdir == "":
		parser.print_help()
		sys.exit(1)
	if not os.path.exists(appdir):
		print "Couldn't find application directory at: %s" % appdir
		sys.exit(1)
	
	magicmarker = os.path.join(cwd,'.packager')
	if os.path.exists(magicmarker):
		options.source = cwd
		options.assets = cwd
		options.packager = True
		
	main(options,args[0])
	
	
