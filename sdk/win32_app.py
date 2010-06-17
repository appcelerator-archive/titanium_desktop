#!/usr/bin/env python
import effess
import os
import os.path as p
import re
import shutil
import string
import sys
import tempfile
import types
import uuid
import subprocess
from app import App
from xml.sax.saxutils import quoteattr

win32_dir = p.join(p.dirname(__file__), 'win32')
if p.exists(win32_dir):
	sys.path.append(win32_dir)
import PyRTF

class Win32App(App):
	def stage(self, stage_dir, bundle):
		App.stage(self, stage_dir, bundle=bundle)

		contents = self.get_contents_dir()
		self.env.log(u'Copying kboot.exe to %s' % contents);
		self.executable_path = p.join(contents, '%s.exe' % self.name)
		effess.copy(p.join(self.sdk_dir, 'kboot.exe'), self.executable_path)

		# The .installed file for Windows should always exist,
		# since we only ever install via the MSI installer.
		open(p.join(contents, '.installed'), 'a').close()

		self.set_executable_icon()

	def set_executable_icon(self):
		if not hasattr(self, 'image'):
			return

		icon_path = str(p.join(self.contents, 'Resources', self.image))
		if not p.exists(icon_path):
			return

		if not(icon_path.lower().endswith('.ico')):
			# Assume that GraphicsMagick is on the path for now. This will change
			# once the packaging server setup has been improved (running on drive C:\)
			convert = 'convert.exe'
			temp_dir = tempfile.mkdtemp()
			new_ico_file = p.join(self.contents, 'Resources', '_converted_icon.ico')
			ico_command = [convert]
			for size in [16, 32, 64, 128]:
				resolution = "%dx%d" % (size, size)
				args = [convert, icon_path, '-resize', resolution + "^",
					"-gravity", "center", "-background", "transparent",
					"-extent", resolution, "%s\\%d.png" % (temp_dir, size)]
				subprocess.check_call(args, shell=True)

				ico_command.append('%s\\%d.png' % (temp_dir, size))

			ico_command.append(new_ico_file)
			subprocess.check_call(ico_command, shell=True)

			icon_path = new_ico_file

		self.env.run('%s "%s" "%s"' % (p.join(self.sdk_dir, 'ReplaceVistaIcon.exe'), self.executable_path, icon_path))

	def package(self, package_dir, bundle):
		contents = self.get_contents_dir()
		target = p.join(package_dir, self.name + '.msi')
		wxs_path = p.join(package_dir, 'installer.wxs')
		template_args = {}

		try:
			template_args['app_name'] = quoteattr(self.name)
			app_version = self.version
			version_parts = len(app_version.split("."))
			if version_parts < 3:
				app_version += ('.0' * (version_parts-1))
			template_args['app_version'] = quoteattr(app_version)
			template_args['app_guid'] = quoteattr(self.guid)
			template_args['app_id'] = quoteattr(self.id)
	
			template_args['app_publisher'] = quoteattr('None provided')
			if hasattr(self, 'publisher'):
				template_args['app_publishe'] = quoteattr(self.publisher)
			template_args['app_description'] = quoteattr('None provided')
			if hasattr(self, 'description'):
				template_args['app_description'] = quoteattr(self.description)
	
			template_args['app_exe'] = quoteattr(p.join(contents, self.name + '.exe'))
	
			license_rtf_path = p.join(contents, 'LICENSE.rtf')
			self.write_license_rtf(license_rtf_path)
			template_args['license_file'] = quoteattr(license_rtf_path)
			template_args['crt_msm'] = quoteattr(p.join(self.sdk_dir, 'installer',
				'Microsoft_VC80_CRT_x86.msm'))
			template_args['titanium_installer_dll'] = quoteattr(p.join(
				self.sdk_dir, 'installer', 'titanium_installer.dll'))
			template_args['dialog_bmp'] = quoteattr(self.get_installer_image(
				'dialog-bmp', p.join(self.sdk_dir, 'default_dialog.bmp')))
			template_args['banner_bmp'] = quoteattr(self.get_installer_image(
				'banner-bmp', p.join(self.sdk_dir, 'default_banner.bmp')))
	
			(app_language, app_codepage) = self.get_app_language()
			template_args['app_language'] = quoteattr(app_language)
			template_args['app_codepage'] = quoteattr(app_codepage)

			root_dir = Directory(self, '.', is_root=True)
			walk_dir(contents, root_dir)
			template_args["app_dirs"] = root_dir.to_xml()

			template_args['component_refs'] = "\n"
			for id in Directory.component_ids:
				template_args['component_refs'] += \
					'\t\t<ComponentRef Id="' + id + '"/>\n'

			template_args['dependencies'] = ''
			if not bundle:
				template_args['dependencies'] = ";".join(self.encode_manifest())

			# Render the WXS template and write it to the WXS file
			# after converting all template arguments to UTF-8
			for (key, value) in template_args.iteritems():
				if type(template_args[key]) == types.UnicodeType:
					template_args[key] = template_args[key].encode('utf8')

			template = string.Template(open(
				p.join(self.sdk_dir, 'installer_template.wxs')).read())

			wxs_file = open(wxs_path, 'w+')
			wxs_text = template.safe_substitute(template_args)
			wxs_file.write(wxs_text)
			wxs_file.close()
			self.env.log(wxs_text.decode('utf8'))

			wix_bin_dir = self.get_wix_bin_directory()
			self.env.run([
				p.join(wix_bin_dir, 'candle.exe'),
				'-out',
				'%s.wixobj' % wxs_path,
				 wxs_path,
			])
			self.env.run([
				p.join(wix_bin_dir, 'light.exe'),
				'-ext', ' WixUIExtension',
				'-out', '%s' % target,
				'%s.wixobj' % wxs_path
			])

		finally:
			self.env.ignore_errors(lambda: os.unlink(wxs_path))
			self.env.ignore_errors(lambda: os.unlink(wxs_path + '.wixobj'))

	def get_wix_bin_directory(self):
		path = p.join("C:\\", "Program Files", "Windows Installer XML v3", "bin")
		if not p.exists(path):
			path = p.join("C:\\", "Program Files (x86)", "Windows Installer XML v3", "bin")
		if not p.exists(path):
			raise Exception('Could not find WiX v3 bin directory')
		return path

	def get_app_language(self):
		self.language = 'en-us'
		self.get_tiapp_element_as_prop('language', 'language')
		langs = {
			"cs-cz": ("1029", "1250"),
			"nl-nl": ("1043", "1252"),
			"en-us": ("1033", "1252"),
			"fr-fr": ("1036", "1252"),
			"de-de": ("1031", "1252"),
			"hu-hu": ("1038", "1250"),
			"it-it": ("1040", "1252"),
			"ja-jp": ("1041", "932" ),
			"pl-pl": ("1045", "1250"),
			"ru-ru": ("1049", "1251"),
			"es-es": ("3082", "1252"),
			"uk-ua": ("1058", "1251")
		}
		if self.language in langs:
			return langs[self.language]
		else:
			return langs['en-us']

	def write_license_rtf(self, rtf_path):
		# PyRTF and PyRTF-ng it seems do not support Unicode types
		# when they do this code should read the file using the codecs
		# module and create a Unicode RTF.
		if not p.exists(rtf_path):
			license_file = p.join(self.get_contents_dir(), 'LICENSE.txt')
			if p.exists(license_file):
				license_text = open(license_file, 'r').read()
			else:
				license_text = 'This software was not shipped with a license.'
			doc = PyRTF.Document()
			section = PyRTF.Section()
			doc.Sections.append(section)
			for paragraph in re.split("\n\n|\r\n\r\n", license_text):
				section.append(paragraph)

			renderer = PyRTF.Renderer()
			renderer.Write(doc, open(rtf_path, 'w'))

	def encode_manifest(self):
		output = []
		def write_line(str):
			output.append(str.encode('utf8'))

		write_line(u'#appname:' + self.name)
		write_line(u'#appid:' + self.id)
		write_line(u'#guid:' + self.guid)
		write_line(u'#version:' + self.version)
		if hasattr(self, 'image'):
			write_line(u'#image:' + self.image)
		if hasattr(self, 'publisher'):
			write_line(u'#publisher:' + self.publisher)
		if hasattr(self, 'description'):
			write_line(u'#description:' + self.description)
		if hasattr(self, 'url'):
			write_line(u'#url:' + self.url)
		if hasattr(self, 'loglevel'):
			write_line(u'#loglevel:' + self.url)
		if hasattr(self, 'stream'):
			write_line(u'#stream:' + self.url)

		write_line(u'runtime:' + self.runtime_version)
		if hasattr(self, 'sdk_version'):
			write_line(u'sdk:' + self.sdk_version)
		if hasattr(self, 'mobilesdk_version'):
			write_line(u'mobilesdk:' + self.mobilesdk_version)
		for module in self.modules:
			write_line(module[0] + ':' + module[1])

		return output


file_template = """
%(indent)s	<File Id="%(id)s_file" Source=%(full_path)s KeyPath="%(keypath)s">
%(shortcuts)s
%(indent)s	</File>
"""

shortcut_template = """
		<Shortcut Id="%(id)s" Directory="%(directory)s" Name="%(name)s"
			WorkingDirectory="%(working_dir)s" Icon="ApplicationIcon.exe"
			IconIndex="0" Advertise="yes" />
"""

def id_generator():
	file_id = 1
	while True:
		yield "_" + str(file_id)
		file_id += 1
unique_ids = id_generator()

class Shortcut(object):
	@classmethod
	def create_start_menu_shortcut(cls, app):
		return Shortcut("ProgramMenuDir", app.name, "INSTALLDIR")
	
	@classmethod
	def create_desktop_shortcut(cls, app):
		return Shortcut("DesktopFolder", app.name, "INSTALLDIR")

	def __init__(self, directory, name, working_dir):
		self.directory = directory
		self.name = name
		self.working_dir = working_dir
	
	def to_xml(self):
		return shortcut_template % {
			"id": unique_ids.next(),
			"directory": self.directory,
			"name": self.name,
			"working_dir": self.working_dir}

class Directory(object):
	component_ids = []

	def __init__(self, app, relative_path, is_root=False):
		self.app = app
		self.relative_path = relative_path
		self.name = os.path.basename(relative_path)
		self.files = []
		self.dirs = []
		self.is_root = is_root

	def add_file(self, relative_path, full_path, shortcuts=None):
		file = {
			"filename": quoteattr(os.path.basename(relative_path)),
			"relative_path": relative_path,
			"id": unique_ids.next(),
			"full_path": quoteattr(full_path)
		}

		# The File attribute containing these shortcuts must be the KeyPath
		# of it's containing component for shortcut advertising to work properly.
		file['shortcuts'] = ''
		file['keypath'] = 'no'
		if relative_path == self.app.name + ".exe":
			file['shortcuts'] += Shortcut.create_start_menu_shortcut(self.app).to_xml()
			file['shortcuts'] += Shortcut.create_desktop_shortcut(self.app).to_xml()
			file['keypath'] = 'yes'
		self.files.append(file)

	def add_dir(self, dir):
		self.dirs.append(dir)

	def to_xml(self, indent=4):
		xml = ""
		if not self.is_root:
			xml += ("\t" * indent) + '<Directory Id="%s" Name="%s">\n' % \
				(unique_ids.next(), self.name)

		if len(self.files) > 0:
			component_id = unique_ids.next()
			Directory.component_ids.append(component_id)
			xml += '<Component Id="%s" Guid="%s">' % \
				(component_id, str(uuid.uuid4()).upper())

			for file in self.files:
				file['indent'] = "\t" * indent
				xml += file_template % file

			xml += '</Component>'
			
		for dir in self.dirs:
			xml += dir.to_xml(indent+1)
		
		if not self.is_root:
			xml += ("\t" * indent) + "</Directory>\n"
		return xml

def walk_dir(path, current_dir, relative_path=""):	
	for file in os.listdir(path):
		if file == "*" or file == "*.*" or file == "." or file == "..":
			continue
		
		file_relative_path = os.path.join(relative_path, file)
		file_full_path = os.path.join(path, file)
		if relative_path == "":
			file_relative_path = file
		
		if os.path.isfile(file_full_path):
			current_dir.add_file(file_relative_path, file_full_path)
		else:
			newdir = Directory(current_dir.app, file_relative_path)
			current_dir.add_dir(newdir)
			walk_dir(file_full_path, newdir, file_relative_path)

