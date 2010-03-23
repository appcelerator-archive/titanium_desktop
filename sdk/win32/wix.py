#!/usr/bin/python
import os, sys, string, shutil
import uuid, re, tempfile, subprocess
import PyRTF
from xml.sax.saxutils import quoteattr

support_dir = os.path.abspath(os.path.dirname(__file__))

# some sane defaults
default_app_language = "1033"
default_app_codepage = "1252"

def read_template(sdk_path, name):
	return open(os.path.join(sdk_path, name), 'r').read()

def gen_guid():
	return str(uuid.uuid4()).upper()

def id_generator():
	file_id = 1
	while True:
		yield "_" + str(file_id)
		file_id += 1
unique_ids = id_generator()

def write_rtf(text, filename):
	doc = PyRTF.Document()
	section = PyRTF.Section()
	doc.Sections.append(section)
	for paragraph in re.split("\n\n|\r\n\r\n", text):
		section.append(paragraph)
	
	renderer = PyRTF.Renderer()
	renderer.Write(doc, open(filename, "w"))

def invalid_language(lang):
	message = "Error: %s is an invalid language. "+\
		"Currently supported languages:\n" % lang
	for l in langs:
		message += "\t%s [%s]\n" % (langs[l][0], l)
	print >>sys.stderr, message
	sys.exit(-1)
	
def get_app_language(lang):
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
	if lang in langs:
		return langs[lang]
	else:
		return langs['en-us']

class Shortcut:
	@classmethod
	def create_start_menu_shortcut(cls, builder):
		return Shortcut("ProgramMenuDir", builder.appname, "INSTALLDIR")
	
	@classmethod
	def create_desktop_shortcut(cls, builder):
		return Shortcut("DesktopFolder", builder.appname, "INSTALLDIR")
			
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
	
class Directory:
	component_ids = []
	
	def __init__(self, builder, relative_path, is_root=False):
		self.builder = builder
		self.relative_path = relative_path
		self.name = os.path.basename(relative_path)
		self.files = []
		self.dirs = []
		self.is_root = is_root
			
	def add_file(self, relative_path, full_path, shortcuts=None):
		file = {
			"guid": gen_guid(),
			"filename": quoteattr(os.path.basename(relative_path)),
			"relative_path": relative_path,
			"id": unique_ids.next(),
			"full_path": quoteattr(full_path)
		}
		
		# Each file is a component which will be referenced later
		# in the list of component references.
		Directory.component_ids.append(file['id'])

		shortcuts_xml = ''
		if relative_path == self.builder.appname + ".exe":
			shortcuts_xml += Shortcut.create_start_menu_shortcut(self.builder).to_xml()
			shortcuts_xml += Shortcut.create_desktop_shortcut(self.builder).to_xml()
		file['shortcuts'] = shortcuts_xml
		
		self.files.append(file)
		
	def add_dir(self, dir):
		self.dirs.append(dir)

	def to_xml(self, indent=4):
		xml = ""
		if not self.is_root:
			xml += ("\t" * indent) + "<Directory Id=\"%s\" Name=\"%s\">\n" % \
				(unique_ids.next(), self.name)

		for file in self.files:
			file['indent'] = "\t" * indent
			xml += component_template % file
			
		for dir in self.dirs:
			xml += dir.to_xml(indent+1)
		
		if not self.is_root:
			xml += ("\t" * indent) + "</Directory>\n"
		return xml
	
def run_command(args):
	subprocess.call(args)
	
def get_from_tiapp(tiapp, name, default_value):
	el = tiapp.find(name)
	if el is None or el is -1:
		return default_value
	return el.text
	
component_template = """
%(indent)s<Component Id="%(id)s_component" Guid="%(guid)s">
%(indent)s	<File Id="%(id)s_file" Source=%(full_path)s KeyPath="yes">
%(shortcuts)s
%(indent)s	</File>
%(indent)s</Component>
"""

shortcut_template = """
		<Shortcut Id="%(id)s" Directory="%(directory)s" Name="%(name)s"
			WorkingDirectory="%(working_dir)s" Icon="ApplicationIcon.exe"
			IconIndex="0" Advertise="yes" />
"""

bundled_template = """
%(indent)s<Directory Id="CommonAppDataFolder" Name="AppData">
%(indent)s	<Directory Id="Titanium" Name="Titanium">
%(indent)s		<Directory Id="modules" Name="modules">
%(indent)s			<Directory Id="modules_win32" Name="win32">
%(indent)s				%(module_dirs)s
%(indent)s			</Directory>
%(indent)s		</Directory>
%(indent)s		<Directory Id="runtime" Name="runtime">
%(indent)s			<Directory Id="runtime_win32" Name="win32">
%(indent)s				<Directory Id="runtime_win32_%(runtime_version)s" Name="%(runtime_version)s">
%(indent)s					%(runtime_files)s
%(indent)s				</Directory>
%(indent)s			</Directory>
%(indent)s		</Directory>
%(indent)s	</Directory>
%(indent)s</Directory>
"""

module_dir_template = """
%(indent)s<Directory Id="module_%(module_id)s" Name="%(module_id)s">
%(indent)s	<Directory Id="module_%(module_id)s_%(module_version)s" Name="%(module_version)s">
%(indent)s		%(module_files)s
%(indent)s	</Directory>
%(indent)s</Directory>
"""
def walk_dir(builder, path, current_dir, relative_path=""):	
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
			newdir = Directory(builder, file_relative_path)
			current_dir.add_dir(newdir)
			walk_dir(builder, file_full_path, newdir, file_relative_path)

def build_msi(template, args, basename, destdir):
	t = string.Template(template)
	wxs = t.safe_substitute(args)
	
	msi = os.path.join(destdir, basename+ ".msi")
	wxsname = os.path.join(tempfile.gettempdir(), basename + ".wxs")
	tmpfile = open(wxsname, "w+")
	tmpfile.write(wxs)
	tmpfile.close()
	print wxs
	
	wix_dir = os.path.join("C:\\", "Program Files", "Windows Installer XML v3", "bin")
	if not os.path.exists(wix_dir):
		wix_dir = os.path.join("C:\\", "Program Files (x86)", "Windows Installer XML v3", "bin")
	if not os.path.exists(wix_dir):
		print >>sys.stderr, "Error: Couldn't find WiX v3 installation dir for creating installer"
		sys.exit(-1)

	candle = os.path.join(wix_dir, "candle.exe")
	light = os.path.join(wix_dir, "light.exe")
	run_command([candle, wxsname, "-out", wxsname+".wixobj"])
	run_command([light, wxsname+".wixobj", "-ext", "WixUIExtension", "-out", msi])

	os.unlink(wxsname)
	os.unlink(wxsname+".wixobj")
	return msi

def create_installer(builder):
	sdk_path = builder.options.assets_dir
	installer_sdk_path = os.path.join(sdk_path, 'installer')
	app_installer_template = read_template(sdk_path, 'app_installer_template.wxs')

	(app_language, app_codepage) = get_app_language(
		get_from_tiapp(builder.options.tiapp, 'language', 'en-us'))
	if app_language is None or app_codepage is None:
		invalid_language(lang)
	
	license_text = "This software was not shipped with a license."
	license_file = get_from_tiapp(builder.options.tiapp,
		'license', os.path.join(builder.options.appdir, 'LICENSE.txt'))
	if os.path.exists(license_file):
		license_text = open(license_file, "r").read()
	license_rtf = os.path.join(tempfile.gettempdir(),
		builder.appname + '_license.rtf')
	write_rtf(license_text, license_rtf)
	
	def get_bmp(tag, default_bmp):	
		bmp = get_from_tiapp(builder.options.tiapp, tag, default_bmp)
		if not os.path.exists(bmp):
			new_bmp = os.path.join(builder.options.appdir,
				bmp.replace("/", "\\"))
			if not os.path.exists(new_bmp):
				error = "Warning: specified %s '%s'" + \
					"wasn't found at %s, reverting to default"
				print >>sys.stderr, error % (tag, bmp, new_bmp)
				return default_bmp
			else:
				return new_bmp
		return bmp

	default_dialog_bmp = os.path.join(sdk_path, 'default_dialog.bmp')
	default_banner_bmp = os.path.join(sdk_path, 'default_banner.bmp')
	dialog_bmp = get_bmp('dialog-bmp', default_dialog_bmp)
	banner_bmp = get_bmp('banner-bmp', default_banner_bmp)

	titanium_installer_dll = os.path.join(installer_sdk_path, 'titanium_installer.dll')
	crt_msm = os.path.join(installer_sdk_path, 'Microsoft_VC80_CRT_x86.msm')
	
	app_version = builder.appversion
	version_parts = len(app_version.split("."))
	if version_parts < 3:
		app_version += ('.0' * (version_parts-1))

	app_guid = builder.options.manifest['guid'].upper()	
	common_args = {
		"app_name": quoteattr(builder.appname),
		"app_exe": quoteattr(os.path.join(builder.base_dir, builder.appname + ".exe")),
		"app_id": quoteattr(builder.options.manifest['appid']),
		"app_guid": quoteattr(app_guid),
		"app_publisher": quoteattr(builder.options.manifest['publisher']),
		"app_description": quoteattr(builder.options.manifest['desc']),
		"app_version": quoteattr(app_version),
		"program_menu_guid": quoteattr(gen_guid()),
		"app_language": app_language,
		"app_codepage": app_codepage,
		"license_rtf": quoteattr(license_rtf),
		"dialog_bmp": dialog_bmp,
		"banner_bmp": banner_bmp,
		"titanium_installer_dll": titanium_installer_dll,
		"crt_msm": crt_msm }
	
	installed_file = os.path.join(builder.base_dir, ".installed")
	open(installed_file, "a").close() #touch
	
	root_dir = Directory(builder, ".", is_root=True)
	walk_dir(builder, builder.base_dir, root_dir)
	
	component_ref_xml = "\n"
	for id in Directory.component_ids:
		component_ref_xml += \
			'\t\t<ComponentRef Id="' + id + '_component"/>\n'
		
	bundled_xml = ""
	modules = builder.options.manifest['modules']
	dependencies = []
	bundled_modules = []
	bundled_runtime = False
	for module in modules:
		version = modules[module]
		dependencies.append("%s=%s" % (module,version))
	for key in builder.options.manifest:
		if key is not 'modules':
			dependencies.append("#%s=%s" % (key, builder.options.manifest[key]))
	
	modules_dir = os.path.join(builder.base_dir, "modules")
	if os.path.exists(modules_dir):
		for dir in os.listdir(modules_dir):
			bundled_modules.append(dir)
	if os.path.exists(os.path.join(builder.base_dir, "runtime")):
		bundled_runtime = True
	
	bundled_modules_xml = ""
	if len(bundled_modules) > 0:
		bundled_modules_xml = '<Property Id="AppBundledModules" Value="%s"/>' % ','.join(bundled_modules)
	
	bundled_runtime_xml = ""
	if bundled_runtime is True:
		bundled_runtime_xml = '<Property Id="AppBundledRuntime" Value="runtime"/>'
	
	app_template_args = common_args.copy()
	app_template_args["app_dirs"] = root_dir.to_xml()
	app_template_args["component_refs"] = component_ref_xml
	app_template_args["bundled_xml"] = bundled_xml
	app_template_args["dependencies"] = "&amp;".join(dependencies)
	app_template_args["bundled_modules"] = bundled_modules_xml
	app_template_args["bundled_runtime"] = bundled_runtime_xml
	
	install_msi = build_msi(app_installer_template, app_template_args,
		builder.appname, builder.options.destination)

	if license_rtf != default_license_rtf:
		os.unlink(license_rtf)

	return install_msi
