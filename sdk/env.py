import app
import osx_app
import linux_app
import os
import platform
import sys
import os.path as p

class PackagingEnvironment(object):
	def __init__(self, version, target_os, product_name="kroll"):
		self.target_os = target_os
		if target_os is 'linux':
			self.App = linux_app.LinuxApp
		if target_os is 'osx':
			self.App = osx_app.App
		if target_os is 'win32':
			self.App = app.Win32App

		self.version = version

		if (target_os is 'linux'):
			pname = product_name.lower()
			self.install_dirs = [
				p.expanduser('~/.' + pname),
				"/opt/" + pname,
				"/usr/local/lib/" + pname,
				"/usr/lib/" + pname
			]
		elif (target_os is 'osx'):
			pname = product_name.capitalize()
			self.install_dirs = [
				p.expanduser('~/Library/Application Support/' + pname),
				'/Library/Application Support/' + pname
			]
		elif (target_os is 'win32'):
			pname = product_name.capitalize()
			self.install_dirs = [
				p.join(os.environ['APPDATA'], pname),
				# TODO: Is there a better way to determine this directory?
				'C:\\ProgramData\\' + pname
			]
		else:
			raise Exception("Unknown environment!")

		# If we are a packaging server, everything we need will
		# be in the same directory as this script file.
		script_dir = p.abspath(p.dirname(sys._getframe(0).f_code.co_filename))

		# If we are in the build hierarchy, try to find runtimes and modules
		# relative to this file's location.
		build_subpath = p.join('build', self.target_os)
		self.components_dir = None
		if (p.exists(p.join(script_dir, '..', 'kroll')) and
			p.exists(p.join(script_dir, '..', 'build', self.target_os, 'runtime')) and
			p.exists(p.join(script_dir, '..', 'build', self.target_os, 'sdk'))):
			self.components_dir = p.join(script_dir, '..', 'build', self.target_os)
		elif p.exists(p.join(script_dir, '..', 'runtime')) and p.exists(p.join(script_dir, '..', 'sdk')):
			self.components_dir = p.join(script_dir, '..')

		# Couldn't find any build assets, so assume that we could be a
		# packaging server, which has assets in the same directory as the
		# script.
		elif p.exists(p.join(script_dir, 'runtime')):
			self.components_dir = cwd

	def log(self, text):
		print u'    -> %s' % text

	def get_excludes(self):
		return ['.pdb', '.exp', '.ilk', '.lib', '.svn',
			'.git', '.gitignore', '.cvsignore']

	def get_component(self, type, name, version):
		# First try the build directory.
		if self.components_dir:
			target = p.join(self.components_dir, type)
			if name: # Modules have names
				 target = p.join(target, name)
			if p.exists(target):
				return target

		# Next try searching list of installed directories
		for dir in self.install_dirs:
			target = p.join(dir, type, self.target_os)
			if name: target = p.join(target, name)
			target = p.join(target, version)
			if p.exists(target):
				return target

		return None

	def get_sdk_dir(self, version):
		c = self.get_component('sdk', None, version)
		if not c:
			raise Exception(u'Could not find SDK version %s' % version)
		return c

	def get_runtime_dir(self, version):
		c = self.get_component('runtime', None, version)
		if not c:
			raise Exception(u'Could not find runtime version %s' % version)
		return c

	def get_module_dir(self, module):
		c = self.get_component('modules', module[0], module[1])
		if not c:
			raise Exception(u'Could not find module %s-%s' % module)
		return c

	def run(self, command):
		self.log(u'Launching: %s' % command)
		os.system(command)
