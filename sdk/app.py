#!/usr/bin/env python
import os.path as p
import codecs
import effess
import xml.etree.ElementTree
from xml.etree.ElementTree import ElementTree

class App(object):
	def __init__(self, env, source_dir):
		self.env = env
		self.source_dir = source_dir # The directory for the source contents
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
		self.sdk_dir = self.env.get_sdk_dir(self.runtime_version)

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
		self.get_tiapp_element_as_prop('publisher', 'publisher')
		self.get_tiapp_element_as_prop('url', 'url')
		self.get_tiapp_element_as_prop('log-level', 'loglevel')

	def write_manifest(self, path):
		f = codecs.open(p.join(path, 'manifest'), 'wb', 'utf-8')
		f.write(u'#appname: ' + self.name + '\n')
		f.write(u'#appid: ' + self.id + '\n')
		f.write(u'#guid: ' + self.guid + '\n')
		f.write(u'#version: ' + self.version + '\n')
		if hasattr(self, 'image'):
			f.write(u'#image: ' + self.image + '\n')
		if hasattr(self, 'publisher'):
			f.write(u'#publisher: ' + self.publisher + '\n')
		if hasattr(self, 'url'):
			f.write(u'#url: ' + self.url + '\n')
		if hasattr(self, 'loglevel'):
			f.write(u'#loglevel: ' + self.url + '\n')
		if hasattr(self, 'stream'):
			f.write(u'#stream: ' + self.url + '\n')

		f.write(u'runtime: ' + self.runtime_version + '\n')
		if hasattr(self, 'sdk_version'):
			f.write(u'sdk: ' + self.sdk_version + '\n')
		if hasattr(self, 'mobilesdk_version'):
			f.write(u'mobilesdk: ' + self.mobilesdk_version + '\n')
		for module in self.modules:
			f.write(module[0] + ': ' + module[1] + '\n')
		f.close()

	def write_tiapp(self, path):
		xml.etree.ElementTree._namespace_map['http://ti.appcelerator.org'] = 'ti'
		xml.etree.ElementTree._namespace_map['http://www.appcelerator.org'] = 'appc'
		self.tiapp.write(p.join(path, 'tiapp.xml'), "utf-8")

	def get_contents_dir(self):
		return self.stage_dir

	def stage(self, stage_dir, bundle=False):
		print('Staging %s' % self.name)
		self.stage_dir = stage_dir
		contents = self.contents = self.get_contents_dir()

		self.env.log(u'Copying contents from %s to %s' % (self.source_dir, contents))
		effess.copy_tree(self.source_dir, contents, exclude=self.env.get_excludes())

		installer_source = p.join(self.sdk_dir, 'installer')
		self.env.log(u'Copying installer from %s to %s' % (installer_source, contents))
		effess.copy_to_dir(installer_source, contents)

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

class Win32App(App):
	def stage(self, stage_dir, bundle):
		App.stage(self, stage_dir, bundle=bundle)

		contents = self.get_contents_dir()
		self.env.log(u'Copying kboot.exe to %s' % contents);
		effess.copy(p.join(self.sdk_dir, 'kboot.exe'),
			p.join(contents, '%s.exe' % self.name))
