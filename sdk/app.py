#!/usr/bin/env python
import os.path as p
import codecs
import effess
import xml.etree.ElementTree
from xml.etree.ElementTree import ElementTree

def fix_path(path):
	return p.abspath(p.expanduser(p.normpath(path)))

class App(object):
	def __init__(self, env, source_dir):
		self.env = env
		self.source_dir = fix_path(source_dir)
		self.modules = []
		self.read_manifest()
		self.read_tiapp()

		# There are some basic attributes that we need to know to continue.
		# If these weren't passed, we should abort here.
		for attr in ['name', 'id', 'guid', 'version', 'runtime_version']:
			if not hasattr(self, attr):
				raise Exception('Neither the tiapp.xml nor the manifest defined "%s"' % attr)

		# Cache the SDK directory for the runtime here, because we'll
		# need it quite a bit if we do staging and packaging.
		self.sdk_dir = fix_path((self.env.get_sdk_dir(self.runtime_version)))

	def read_manifest(self):
		manifest_path = p.join(self.source_dir, "manifest")
		if not p.exists(manifest_path):
			raise Exception("Could not find manifest at %s" % manifest_path)

		contents = codecs.open(manifest_path, 'r', 'utf-8').read().split('\n')
		for line in contents:
			if line.find(u':') == -1:
				continue
			(key, value) = line.split(':', 1)
			key = key.strip()
			value = value.strip()
			if key == u'#appname': self.name = value
			elif key == u'#appid': self.id = value
			elif key == u'#guid': self.guid = value
			elif key == u'#description': self.description = value
			elif key == u'#image': self.image = value
			elif key == u'#publisher': self.publisher = value
			elif key == u'#url': self.url = value
			elif key == u'#version': self.version = value
			elif key == u'#loglevel': self.loglevel = value
			elif key == u'#stream': self.stream = value
			elif key.find(u'#') == 0: continue
			else:
				# This is for staging applications in our source directory.
				# If no version is specified for this application, just use
				# the current build environment version.
				if not value:
					value = self.env.version

				if key == u'runtime': self.runtime_version = value
				elif key == u'sdk': self.sdk_version = value
				elif key == u'mobilesdk': self.mobilesdk_version = value
				else:
					self.modules.append((key, value))

	def get_tiapp_element_as_prop(self, element_name, prop):
		t = self.tiapp.findtext(element_name)
		if t: self.__setattr__(prop, unicode(t))

	def read_tiapp(self):
		tiapp_path = p.join(self.source_dir, "tiapp.xml")
		if not p.exists(tiapp_path):
			raise Exception("Could not find tiapp.xml at %s" % tiapp_path)

		self.tiapp = ElementTree()
		self.tiapp.parse(tiapp_path)
		self.get_tiapp_element_as_prop('name', 'name')
		self.get_tiapp_element_as_prop('id', 'id')
		self.get_tiapp_element_as_prop('version', 'version')
		self.get_tiapp_element_as_prop('icon', 'image')
		self.get_tiapp_element_as_prop('description', 'description')
		self.get_tiapp_element_as_prop('publisher', 'publisher')
		self.get_tiapp_element_as_prop('url', 'url')
		self.get_tiapp_element_as_prop('log-level', 'loglevel')

	def write_manifest(self, path):
		f = codecs.open(p.join(path, 'manifest'), 'wb', 'utf-8')

		def write_line(str):
			f.write(str.replace(u'\n', u'') + '\n')

		write_line(u'#appname: ' + self.name)
		write_line(u'#appid: ' + self.id)
		write_line(u'#guid: ' + self.guid)
		write_line(u'#version: ' + self.version)
		if hasattr(self, 'image'):
			write_line(u'#image: ' + self.image)
		if hasattr(self, 'publisher'):
			write_line(u'#publisher: ' + self.publisher)
		if hasattr(self, 'description'):
			write_line(u'#description: ' + self.description)
		if hasattr(self, 'url'):
			write_line(u'#url: ' + self.url)
		if hasattr(self, 'loglevel'):
			write_line(u'#loglevel: ' + self.url)
		if hasattr(self, 'stream'):
			write_line(u'#stream: ' + self.url)

		write_line(u'runtime: ' + self.runtime_version)
		if hasattr(self, 'sdk_version'):
			write_line(u'sdk: ' + self.sdk_version)
		if hasattr(self, 'mobilesdk_version'):
			write_line(u'mobilesdk: ' + self.mobilesdk_version)
		for module in self.modules:
			write_line(module[0] + ': ' + module[1])
		f.close()

	def install(self):
		f = open(p.join(self.get_contents_dir(), '.installed'), 'w')
		f.write("installed")
		f.close()

	def write_tiapp(self, path):
		xml.etree.ElementTree._namespace_map['http://ti.appcelerator.org'] = 'ti'
		xml.etree.ElementTree._namespace_map['http://www.appcelerator.org'] = 'appc'
		self.tiapp.write(p.join(path, 'tiapp.xml'), "utf-8")

	def get_contents_dir(self):
		return self.stage_dir

	def stage(self, stage_dir, bundle=False):
		print('Staging %s' % self.name)
		self.stage_dir = fix_path(stage_dir)
		contents = self.contents = self.get_contents_dir()

		self.env.log(u'Copying contents from %s to %s' % (self.source_dir, contents))
		# If we are staging into a subdirectory of the original
		# application directory (like Titanium Developer), then
		# ignore the immediate child of the original app directory
		# on the way to the stagin directory. Example:
		# App directory: /tmp/MyProject
		# Staging directory: /tmp/MyProject/dist/linux/MyProject
		# then we ignore: /tmp/MyProject/dist
		excludes = self.env.get_excludes()
		if contents.find(self.source_dir) != -1 and \
				contents != self.source_dir:
			(current, child) = p.split(contents)
			while current != self.source_dir:
				(current, child) = p.split(current)
			excludes.append(p.join(current, child))
		effess.copy_tree(self.source_dir, contents, exclude=self.env.get_excludes())

		installer_source = p.join(self.sdk_dir, 'installer')
		self.env.log(u'Copying installer from %s to %s' % (installer_source, contents))
		effess.copy_to_dir(installer_source, contents, exclude=self.env.get_excludes() + ['.dll', '.msm'])

		self.write_manifest(contents)
		self.write_tiapp(contents)

		if bundle:
			self.env.log(u'Copying runtime to %s' % self.stage_dir)
			effess.copy_to_dir(self.env.get_runtime_dir(self.runtime_version),
				contents, exclude=self.env.get_excludes())

			if hasattr(self, 'sdk_version'):
				self.env.log(u'Copying SDK to %s' % contents)
				effess.copy_to_dir(self.sdk_dir, contents,
					exclude=self.env.get_excludes())

			# We don't bundle the MobileSDK currently.
			#if hasattr(self, 'mobilesdk_version'):
			#	self.env.log(u'Copying MobileSDK to %s' % contents)
			#	effess.copy_to_dir(self.env.get_mobilesdk_dir(self.sdk_version),
			#		contents, exclude=self.env.get_excludes())

			for module in self.modules:
				try:
					source = self.env.get_module_dir(module)
				except Exception, e: # Couldn't find module directory.

					# If the module exists in the source directory, the app likely
					# ships with a custom module. In this case, eat the error.
					if not(p.exists(p.join(self.source_dir, 'modules', module[0]))):
						raise e
				target = p.join(contents, 'modules', module[0])
				effess.lightweight_copy_tree(source, target,
					exclude=self.env.get_excludes())

	def run(self):
		self.env.run(self.executable_path)

	def get_installer_image(self, tag_name, default=None):
		# Try to find 'tag_name' and also 'tag-name' (typical XML style)
		elem = self.tiapp.findtext(tag_name)
		if not elem:
			elem = self.tiapp.findtext(tag_name.replace('_', '-'))
		if not elem:
			return default

		# Try to find the image in both the Contents and Resources directories.
		image = unicode(elem)
		if p.exists(p.join(self.contents, image)):
			return p.join(self.contents, image)
		if p.exists(p.join(self.contents, 'Resources', image)):
			return p.join(self.contents, 'Resources', image)
		else:
			self.env.log("Could not find %s: %s. Using default." % \
				(tag_name, default))
			return default
