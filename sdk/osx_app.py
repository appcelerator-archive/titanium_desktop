#!/usr/bin/env python
import effess
import os
import shutil
from app import App
import os.path as p

class OSXApp(App):
	def get_contents_dir(self):
		return p.join(self.stage_dir, 'Contents')

	def stage(self, stage_dir, bundle):
		if not stage_dir.endswith('.app'):
			stage_dir += '.app'

		App.stage(self, stage_dir, bundle=bundle)

		self.env.log(u'Copying kboot to %s' % self.contents)
		self.executable_path = p.join(self.contents, 'MacOS', self.name)
		effess.copy(p.join(self.sdk_dir, 'kboot'), self.executable_path)

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
			self.env.run([
				p.join(self.sdk_dir, 'makeicns'),
				'-in', p.join(self.contents, 'Resources', self.image),
				'-out', p.join(lproj_dir, 'titanium.icns')
			])
		else:
			effess.copy_to_dir(p.join(self.sdk_dir, 'titanium.icns'), lproj_dir)

		# The installer also needs to have the application icon as well.
		effess.copy_to_dir(p.join(lproj_dir, 'titanium.icns'),
			p.join(self.contents, 'installer',' Installer App.app', 'Contents',
				'Resources', 'English.lproj'))

	def package(self, package_dir, bundle=False):
		target = p.join(package_dir, self.name + ".dmg")
		self.env.log("Running pkg-dmg %s => %s" % (self.stage_dir, target))
		if p.exists(target): # Just in case.
			self.env.ignore_errors(lambda: os.unlink(target))

		dmg_background = self.get_installer_image('dmg_background',
			p.join(self.sdk_dir, 'background.jpg'))
		self.env.run([p.join(self.sdk_dir, 'pkg-dmg'),
			'--source', '%s' % self.stage_dir,
			'--target', '%s' % target, # Target DMG
			'--volname', '%s' % self.name, # Volume name
			'--format', 'UDBZ',
			'--sourcefile', # This --source argument should be a child of the root
			'--copy', '%s/ds_store:/.DS_Store' % self.sdk_dir, # Copy in .DS_Store
			'--mkdir', '/.background', # Create a .background folder
			'--copy', '%s:/.background/background.jpg' % dmg_background,
			'--symlink', '/Applications:/Applications'
		])

