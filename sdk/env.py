import app
import osx_app
import linux_app
import win32_app
import os
import platform
import subprocess
import sys
import os.path as p
import __init__

class PackagingEnvironment(object):
	def __init__(self, target_os):
		self.version = __init__.get_titanium_version()
		self.excludes = ['.pdb', '.exp', '.ilk', '.lib', '.svn',
			'.git', '.gitignore', '.cvsignore']

		self.target_os = target_os
		if (target_os is 'linux'):
			self.install_dirs = [
				p.expanduser('~/.titanium'),
				"/opt/titanium",
				"/usr/local/lib/titanium",
				"/usr/lib/titanium"
			]
		elif (target_os is 'osx'):
			self.install_dirs = [
				p.expanduser('~/Library/Application Support/Titanium'),
				'/Library/Application Support/Titanium'
			]
		elif (target_os is 'win32'):
			self.install_dirs = [
				p.join(os.environ['APPDATA'], 'Titanium'),
				# TODO: Is there a better way to determine this directory?
				'C:\\ProgramData\\Titanium'
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

	def create_app(self, path):
		if self.target_os is 'linux':
			return linux_app.LinuxApp(self, path)
		if self.target_os is 'osx':
			return osx_app.OSXApp(self, path)
		if self.target_os is 'win32':
			return win32_app.Win32App(self, path)

	def log(self, text):
		print u'    -> %s' % text
		sys.stdout.flush()

	def get_excludes(self):
		return self.excludes

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

	def run(self, args):
		self.log(u'Launching: %s' % args)
		subprocess.call(args)

	def ignore_errors(self, function):
		try:
			function()
		except Exception, e:
			self.log("Ignoring error: %s" % e)
