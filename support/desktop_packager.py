#
# This script is the property of Appcelerator, Inc. and
# is Confidential and Proprietary. All Rights Reserved.
# Redistribution without expression written permission
# is not allowed.
#
# Titanium application desktop Packager class
#
# Original author: Jeff Haynie 04/02/09
#
#
import os, shutil, distutils.dir_util as dir_util, zipfile, tarfile, time, sys
import glob

class DesktopPackager(object):
	def __init__(self, builder):
		self.options = builder.options
		self.log = builder.log
		self.appname = builder.appname
		
		self.log("Executing desktop packager")
		
		if self.options.package:
			if self.options.platform == 'osx':
				self.package = self.create_dmg(builder)
			elif builder.options.platform == 'linux':
				self.package = self.create_tgz(builder)
			elif builder.options.platform == 'win32':
				self.package = self.create_zip(builder)

	def create_zip(self, builder):
		extractor = os.path.join(self.options.assets_dir, 'self_extractor.exe')
		exe = builder.options.executable
		shutil.copy(extractor,exe)
		builder.log("making win32 binary at %s, this will take a sec..." % exe)
		zf = zipfile.ZipFile(exe, 'a', zipfile.ZIP_DEFLATED)

		# add the shitty ass MSVCRT crap
		for f in glob.glob(os.path.join(self.options.runtime_dir,'Microsoft.VC80.CRT')+'/*'):
			zf.write(f,'Microsoft.VC80.CRT/'+os.path.basename(f))

		kboot = os.path.join(builder.options.runtime_dir, 'template', 'kboot.exe')
		installer = os.path.join(builder.options.runtime_dir, 'installer', 'Installer.exe')
		
		zf.write(kboot,'template/kboot.exe')
		zf.write(kboot, builder.appname + '.exe')
		zf.write(installer, 'installer/Installer.exe')
		for walk in os.walk(builder.base_dir):
			for file in walk[2]:
				file = os.path.join(walk[0], file)
				if file != exe:
					arcname = file.replace(builder.base_dir, "")
					builder.log("Adding " + arcname)
					zf.write(file, arcname)
		zf.close()
		return exe

	def walk_dir(self, dir, callback):
		files = os.walk(dir)
		for walk in files:
			for file in walk[2]:
				callback(os.path.join(walk[0], file))
				
	def create_tgz(self, builder):
		outtarfile = os.path.join(builder.options.destination, builder.appname + '.tgz')
		tar = tarfile.open(outtarfile, 'w:gz')
		def tarcb(f):
			arcname = f.replace(builder.base_dir + os.sep, "")
			arcname = builder.appname + "-" + builder.appversion + "/" + arcname
			tar.add(f, arcname)
		self.walk_dir(builder.base_dir, tarcb)
		tar.close()
		return outtarfile

	def folder_size_in_MB(self,folder,add_to_it=0):
		folder_size = add_to_it
		for (path, dirs, files) in os.walk(folder):
		  for file in files:
		    filename = os.path.join(path, file)
		    folder_size += os.path.getsize(filename)

		return folder_size/(1024*1024.0)


	def create_dmg(self,builder):	

		dmg = os.path.join(self.options.destination, builder.appname)
		temp_dmg = dmg+'_'

		app_icns = builder.app_icns

		volname = os.path.join(self.options.destination,'mnt')
		if os.path.exists(volname):
			try:
				builder.invoke("hdiutil detach \"%s\" 2>/dev/null" % volname)
			except OSError:
				True
			
			try:
				if os.path.exists(volname):
					os.unlink(volname)
			except OSError:
				True
			shutil.rmtree(volname)
			
		os.makedirs(volname)


		for d in [temp_dmg,dmg]:
			n = d+'.dmg'
			if os.path.isfile(n):
				os.remove(n)

		self.log("DMG Source: %s" % builder.base_dir)
		self.log("Temp DMG: %s.dmg" % temp_dmg)
		self.log("Building: %s.dmg" % dmg)
		
		# get the size of the image
		add_to_size = 0
		if self.options.dmg_background: add_to_size = os.path.getsize(builder.dmg_background)
		disk_size_in_MB = 10 + self.folder_size_in_MB(builder.base_dir,add_to_size)

		# now run the DMG packager
		builder.invoke("hdiutil create -srcfolder \"%s\" -scrub -volname \"%s\" -fs HFS+ -fsargs \"-c c=64,a=16,e=16\" -megabytes %d -format UDRW \"%s\"" % (builder.base_dir,builder.appname,disk_size_in_MB,temp_dmg))
 		builder.invoke("hdiutil attach -mountpoint \"%s\" -noverify -noautofsck -nobrowse -readwrite -noverify -noautoopen \"%s.dmg\"" % (volname,temp_dmg))
		builder.invoke("bless --folder \"%s\" --openfolder \"%s\"" % (volname,volname))
		time.sleep(1) # give it a chance to update

		builder.invoke("ln -s /Applications \"%s/Applications\"" % (volname))
		
		ds_store = os.path.join(self.options.assets_dir, 'ds_store')
		shutil.copy(ds_store,os.path.join(volname,".DS_Store"))

		imagedir = os.path.join(volname,'.background')
		os.makedirs(imagedir)
		if self.options.dmg_background:
			dmg_bg_ext = os.path.splitext(builder.dmg_background)[1]
			imgdest = os.path.join(imagedir,'background'+dmg_bg_ext)
			builder.invoke("ditto \"%s\" \"%s\"" % (builder.dmg_background,imgdest))
		else:
			dmg_bg_ext = '.jpg'
			builder.invoke("ditto \"%s/background.jpg\" \"%s\"" % (self.options.assets_dir,imagedir))

		builder.invoke("ditto \"%s\" \"%s/.VolumeIcon.icns\"" % (app_icns,volname))
		builder.invoke("/Developer/Tools/SetFile -a C \"%s\"" % volname)
		builder.invoke("/Developer/Tools/SetFile -a V \"%s/.background/background.%s\"" % (volname,dmg_bg_ext))
		
		builder.invoke("hdiutil detach \"%s\"" % volname)
		builder.invoke("hdiutil convert \"%s.dmg\" -format UDBZ -imagekey zlib-level=9 -o \"%s.dmg\"" % (temp_dmg,dmg))
 		os.remove(temp_dmg+'.dmg')

		self.log("osx packager created: \"%s.dmg\"" % dmg)

		try:
			builder.invoke("hdiutil detach \"%s\" 2>/dev/null" % volname)
		except OSError:
			True

		return '%s.dmg' % dmg
