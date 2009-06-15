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
		#exe = os.path.join(self.options.destination,builder.options.executable)
		exe = builder.options.executable
		shutil.copy(extractor,exe)
		builder.log("making win32 binary at %s, this will take a sec..." % exe)
		zf = zipfile.ZipFile(exe, 'a', zipfile.ZIP_DEFLATED)

		# add the shitty ass MSVCRT crap
		for f in glob.glob(os.path.join(self.options.assets_dir,'Microsoft.VC80.CRT')+'/*'):
			zf.write(f,'Microsoft.VC80.CRT/'+os.path.basename(f))

		kboot = os.path.join(builder.options.runtime_dir, 'template', 'kboot.exe')
		installer = os.path.join(builder.options.runtime_dir, 'installer', 'Installer.exe')
		
		zf.write(kboot,'template/kboot.exe')
		zf.write(kboot, builder.appname + '.exe')
		zf.write(installer, 'installer/Installer.exe')
		for walk in os.walk(builder.options.appdir):
			for file in walk[2]:
				file = os.path.join(walk[0], file)
				arcname = file.replace(builder.options.appdir, "")
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

		# now run the DMG packager
		builder.invoke("hdiutil create -srcfolder \"%s\" -scrub -volname \"%s\" -fs HFS+ -fsargs \"-c c=64,a=16,e=16\" -format UDRW \"%s\"" % (builder.base_dir,builder.appname,temp_dmg))
		builder.invoke("hdiutil attach -mountpoint \"%s\" -noverify -noautofsck -nobrowse -readwrite -noverify -noautoopen \"%s.dmg\"" % (volname,temp_dmg))
		builder.invoke("bless --folder \"%s\" --openfolder \"%s\"" % (volname,volname))
		time.sleep(1) # give it a chance to update

		builder.invoke("ln -s /Applications \"%s/Applications\"" % (volname))

		imagedir = os.path.join(volname,'.background')
		os.makedirs(imagedir)
		if self.options.dmg_background:
			dmg_bg_ext = os.path.splitext(builder.dmg_background)[1]
			imgdest = os.path.join(imagedir,'background'+dmg_bg_ext)
			builder.invoke("ditto \"%s\" \"%s\"" % (builder.dmg_background,imgdest))
		else:
			dmg_bg_ext = '.jpg'
			builder.invoke("ditto \"%s/background.jpg\" \"%s\"" % (self.options.assets_dir,imagedir))

		OSA = """
tell application "Finder"
	set f to POSIX file ("%s" as string) as alias
	tell folder f
		open
		tell container window
			set toolbar visible to false
			set statusbar visible to false
			set current view to icon view
			delay 1 -- Sync
			set the bounds to {300, 200, 1000, 1000}
		end tell
		delay 1 -- Sync
		set icon size of the icon view options of container window to 128
		set arrangement of the icon view options of container window to not arranged
		set background picture of the icon view options of container window to file ".background:background%s"
		set position of item "%s.app" to {120, 180}
		set position of item "Applications" to {350, 180}
		set the bounds of the container window to {50, 100, 520, 550}
		set shows item info of the icon view options of container window to false
		set position of the container window to {395, 190}
		update without registering applications
		delay 5 -- Sync
		close
	end tell
	-- Sync
	delay 5
end tell
""" % (os.path.abspath(volname), dmg_bg_ext, self.appname)
		
		
		osa = os.path.join(self.options.destination,'osa')
		osafile = open(osa,'w+')
		osafile.write(OSA)
		osafile.close()
		self.log("Executing finder configuration ... one moment")
		self.log("OSAscript: \n%s" % OSA)
		builder.invoke("osascript \"%s\"" % osa)

		builder.invoke("ditto \"%s\" \"%s/.VolumeIcon.icns\"" % (app_icns,volname))
		builder.invoke("/Developer/Tools/SetFile -a C \"%s\"" % volname)
		builder.invoke("/Developer/Tools/SetFile -a V \"%s/.background/background.jpg\"" % volname)

		bin = os.path.join(volname,"%s.app" % self.appname,"Contents","MacOS",self.appname)
		builder.invoke("lipo -thin i386 \"%s\" -output \"%s\"" % (bin,bin))
		
		builder.invoke("hdiutil detach \"%s\"" % volname)
		builder.invoke("hdiutil convert \"%s.dmg\" -format UDBZ -imagekey zlib-level=9 -o \"%s.dmg\"" % (temp_dmg,dmg))
		os.remove(temp_dmg+'.dmg')

		self.log("osx packager created: \"%s.dmg\"" % dmg)

		try:
			builder.invoke("hdiutil detach \"%s\" 2>/dev/null" % volname)
		except OSError:
			True

		return '%s.dmg' % dmg
