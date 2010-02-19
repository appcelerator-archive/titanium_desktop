#!/usr/bin/env python
import effess
import os
import shutil
from app import App
import os.path as p

def ignore_errors(function):
	try:
		function()
	except Exception, e:
		print "Ignoring error: %s" % e

class OSXApp(App):
	def get_contents_dir(self):
		return p.join(self.stage_dir, 'Contents')

	def stage(self, stage_dir, bundle):
		if not stage_dir.endswith('.app'):
			stage_dir += '.app'

		App.stage(self, stage_dir, bundle=bundle)

		self.env.log(u'Copying kboot to %s' % self.contents)
		effess.copy(p.join(self.sdk_dir, 'kboot'),
			p.join(self.contents, 'MacOS', self.name))

		self.env.log(u'Copying Mac resources to %s' % self.contents)
		# Copy Info.plist to Contents
		plist_file = p.join(self.contents, 'Info.plist')
		effess.copy(p.join(self.sdk_dir, 'Info.plist'), plist_file)
		effess.replace_vars(plist_file, {
			'APPEXE': self.name,
			'APPNAME': self.name,
			'APPICON': 'titanium.icns',
			'APPID': self.id,
			'APPNIB': 'MainMenu',
			'APPVER': self.version
		})

		lproj_dir = p.join(self.contents, 'Resources', 'English.lproj')
		effess.copy_to_dir(p.join(self.sdk_dir, 'MainMenu.nib'), lproj_dir)

		# If there is an icon defined, create a custom titanium.icns file
		if hasattr(self, 'image'):
			self.env.run('"%s" -in "%s" -out "%s"' % (
				p.join(self.sdk_dir, 'makeicns'),
				p.join(self.contents, 'Resources', self.image),
				p.join(lproj_dir, 'titanium.icns')))
		else:
			effess.copy_to_dir(p.join(self.sdk_dir, 'titanium.icns'), lproj_dir)

		# The installer also needs to have the application icon as well.
		effess.copy_to_dir(p.join(lproj_dir, 'titanium.icns'),
			p.join(self.contents, 'installer',' Installer App.app', 'Contents',
				'Resources', 'English.lproj'))

	def get_dmg_background(self):
		background_file = p.join(self.sdk_dir, 'background.jpg')
		elem = self.tiapp.findtext('dmg_background')
		if not elem:
			return background_file

		# Try to find the DMG background relative to the Contents directory
		# or the Resources directory.
		background = unicode(elem)
		if p.exists(p.join(self.contents, background)):
			return p.join(self.contents, background)

		if p.exists(p.join(self.contents, 'Resources', background)):
			return p.join(self.contents, 'Resources', background)

	def package(self, package_dir):
		target = p.join(package_dir, self.name + ".dmg")
		self.env.log("Running pkg-dmg %s => %s" % (self.stage_dir, target))
		if p.exists(target): # Just in case.
			ignore_errors(lambda: os.unlink(target))
		self.env.run('"%s"' % p.join(self.sdk_dir, 'pkg-dmg') +
			' --source "%s"' % self.stage_dir +
			' --target "%s"' % target + # Target DMG
			' --volname "%s"' % self.name + # Volume name
			' --format UDBZ'
			' --sourcefile ' # This --source argument should be a child of the root
			' --copy "%s/ds_store:/.DS_Store"' % self.sdk_dir + # Copy in .DS_Store
			' --mkdir /.background' # Create a .background folder
			' --copy "%s/background.jpg:/.background/background.jpg"' % self.sdk_dir +
			' --symlink /Applications:/Applications')

