#
# This script is the property of Appcelerator, Inc. and
# is Confidential and Proprietary. All Rights Reserved.
# Redistribution without expression written permission
# is not allowed.
#
# Titanium application desktop builder class
#
# Original author: Jeff Haynie 04/02/09
#
#
import os, shutil, distutils.dir_util as dir_util, sys, copy
default_ignore_patterns = ['.git', '.svn', '.gitignore', '.cvsignore']

class DesktopBuilder(object):
	def __init__(self, options, log):
		self.options = options
		self.logger = log
		self.appname = options.manifest['appname']
		self.appversion = options.version
		log(options,"Packaging application named: %s, version: %s"%(self.appname,options.version))
		if options.platform == 'osx':
			self.base_dir = os.path.join(options.destination,('%s.app'%self.appname))
			if not os.path.exists(self.base_dir):
				os.makedirs(self.base_dir)
			self.contents_dir = os.path.join(self.base_dir,'Contents')
			if os.path.exists(self.contents_dir):
				dir_util.remove_tree(self.contents_dir)
			os.makedirs(self.contents_dir)
			self.executable_dir = os.path.join(self.contents_dir,'MacOS')
			os.makedirs(self.executable_dir)
			self.resources_dir = os.path.join(self.contents_dir,'Resources')
			os.makedirs(self.resources_dir)
			self.lproj = os.path.join(self.resources_dir, 'English.lproj')
			os.makedirs(self.lproj)
		else:
			self.base_dir = os.path.join(options.destination,self.appname)
			self.contents_dir = os.path.join(options.destination,self.appname)
			self.executable_dir = os.path.join(options.destination,self.appname)
			if os.path.exists(self.contents_dir):
				dir_util.remove_tree(self.contents_dir)
			if not os.path.exists(self.base_dir):
				os.makedirs(self.base_dir)
			self.resources_dir = os.path.join(self.contents_dir,'Resources')
			os.makedirs(self.resources_dir)

		# copy the tiapp
		tiapp = os.path.join(options.appdir,'tiapp.xml')
		shutil.copy(tiapp, self.contents_dir)
		
		# copy the manifest
		manifest = os.path.join(options.appdir,'manifest')
		shutil.copy(manifest, self.contents_dir)

		# copy the boot
		if options.platform == 'win32':
			kboot = os.path.join(options.runtime_dir, 'template', 'kboot.exe')
			options.executable = os.path.join(self.executable_dir, self.appname+'.exe')
			shutil.copy(kboot, options.executable)
		else:
			kboot = os.path.join(options.runtime_dir, 'template', 'kboot')
			options.executable = os.path.join(self.executable_dir, self.appname)
			shutil.copy(kboot, options.executable)

		rdir = os.path.join(options.appdir,'Resources')
		
		# ignore some common file/directory patterns
		ignore_patterns = copy.copy(default_ignore_patterns)
		if len(options.ignore_patterns) > 0:
			ignore_patterns += options.ignore_patterns.split(",")

		# this is a copy of shutil.copytree, with a check
		# that ignores when the top level dir already exists		
		def copytree(src, dst, symlinks=False, ignore=None):
			names = os.listdir(src)
			if ignore is not None:
				ignored_names = ignore(src, names)
			else:
				ignored_names = set()
			
			if not os.path.exists(dst): os.makedirs(dst)
			errors = []
			for name in names:
				if name in ignored_names:
					continue
				srcname = os.path.join(src, name)
				dstname = os.path.join(dst, name)
				try:
					if symlinks and os.path.islink(srcname):
						linkto = os.readlink(srcname)
						os.symlink(linkto, dstname)
					elif os.path.isdir(srcname):
						copytree(srcname, dstname, symlinks, ignore)
					else:
						shutil.copy2(srcname, dstname)
					# XXX What about devices, sockets etc.?
				except (IOError, os.error), why:
					errors.append((srcname, dstname, str(why)))
				# catch the Error from the recursive copytree so that we can
				# continue with other files
				except Error, err:
					errors.extend(err.args[0])
			try:
				shutil.copystat(src, dst)
			except WindowsError:
				# can't copy file access times on Windows
				pass
			except OSError, why:
				errors.extend((src, dst, str(why)))
			if errors:
				raise Error(errors)
				
 
		def ignore_callback(dir, files):
			# return a list of files to ignore.
			# if the dir is ignored return the full list
			if dir in ignore_patterns:
				return files
			
			file_list = copy.copy(files)
			for file in files:
				if file not in ignore_patterns:
					file_list.remove(file)
			return file_list
		
		# copy in the resources	
		copytree(rdir, self.resources_dir, symlinks=True, ignore=ignore_callback)

		if options.platform == 'osx':
			shutil.copy(os.path.join(options.assets_dir, 'titanium.icns'), self.lproj)
			shutil.copy(os.path.join(options.runtime_dir, 'template','MainMenu.nib'), self.lproj)
			self.app_icns = os.path.join(self.lproj,'titanium.icns')
			self.make_osx_icon(os.path.join(self.resources_dir,options.icon),self.app_icns)
			if options.dmg_background:
				bg = os.path.abspath(os.path.join(options.appdir,'Resources',options.dmg_background))
				if os.path.exists(bg):
					self.dmg_background = bg
					log(options,"Found custom DMG background at %s" % bg)
					
		# copy the installer
		if options.platform == 'osx':
			installer = os.path.join(self.contents_dir,'installer','Installer App.app')
			if not os.path.exists(installer):
				os.makedirs(installer)
			netinstaller = os.path.join(options.runtime_dir, 'installer','Installer App.app')
			dir_util.copy_tree(netinstaller,installer,preserve_symlinks=True)
			# copy in the custom app icon into the net installer
			shutil.copy(self.app_icns,os.path.join(installer,'Contents','Resources','English.lproj'))
		elif options.platform == 'linux':
			installer = os.path.join(self.contents_dir,'installer')
			if not os.path.exists(installer):
				os.makedirs(installer)
			netinstaller = os.path.join(options.runtime_dir, 'installer', 'installer')
			shutil.copy(netinstaller, installer)

		# if selected, write in the .installed file
		if options.no_install:
			f = open(os.path.join(self.contents_dir,'.installed'),'w')	
			f.write("")
			f.close()
		else:
			# copy the license
			if options.license_file:
				f = open(os.path.join(self.contents_dir,'LICENSE.txt'),'w')	
				f.write(open(options.license_file).read())
				f.close()

		# copy in modules (this will be empty if network and no local ones)
		for p in options.module_paths:
			log(options,"adding module: %s/%s"%(p['name'],p['version']))
			d = os.path.join(self.contents_dir,'modules',p['name'])
			if os.path.exists(d):
				dir_util.remove_tree(d)
			os.makedirs(d)
			dir_util.copy_tree(p['path'], d, preserve_symlinks=True)
		
		# copy in the runtime if not network install
		if options.type != 'network':
			log(options,"adding runtime: %s"%options.runtime)
			rd = os.path.join(self.contents_dir,'runtime')
			if os.path.exists(rd):
				dir_util.remove_tree(rd)
			os.makedirs(rd)
			dir_util.copy_tree(options.runtime_dir, rd, preserve_symlinks=True)

		if options.platform == 'osx':
			plist = open(os.path.join(options.assets_dir, 'Info.plist')).read()
			plist = plist.replace('APPNAME', self.appname)
			plist = plist.replace('APPEXE', self.appname)
			plist = plist.replace('APPICON', 'titanium.icns')
			plist = plist.replace('APPID', options.manifest['appid'])
			plist = plist.replace('APPNIB', 'MainMenu')
			plist = plist.replace('APPVER', str(options.version))
			out_file = open(os.path.join(self.contents_dir, 'Info.plist'), 'w')
			out_file.write(plist)
			out_file.close()

	def log(self,msg):
		self.logger(self.options,msg)

	def make_osx_icon(self,icon,dest):
		makeicns = os.path.join(self.options.assets_dir,'makeicns')
		if os.path.exists(makeicns):
			self.invoke('"%s" -in "%s" -out "%s"' % (makeicns,icon,dest))

	def invoke(self,cmd):
		if self.options.verbose:
			self.log("Executing: %s" % cmd)
			os.system(cmd)
		else:
			if self.options.platform == 'win32':
				os.system("%s >NUL" % cmd)
			else:
				os.system("%s >/dev/null" % cmd)

				
