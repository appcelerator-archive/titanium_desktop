#!/usr/bin/python
import os, sys, string, shutil
import uuid, re, tempfile, subprocess
import PyRTF

support_dir = os.path.abspath(os.path.dirname(__file__))

# some sane defaults
default_app_language = "1033"
default_app_codepage = "1252"
default_license_rtf = os.path.join(support_dir, "LICENSE.rtf")
default_dialog_bmp = os.path.join(support_dir, 'default_dialog.bmp')
default_banner_bmp = os.path.join(support_dir, 'default_banner.bmp')

def read_template(name):
	return open(os.path.join(support_dir, name), 'r').read()

def gen_guid():
	return str(uuid.uuid4()).upper()

file_id = 1
def get_file_id():
	global file_id
	file_id += 1
	return "_" + str(file_id)

def write_rtf(text, filename):
	doc = PyRTF.Document()
	section = PyRTF.Section()
	doc.Sections.append(section)
	for paragraph in re.split("\n\n|\r\n\r\n", text):
		section.append(paragraph)
	
	renderer = PyRTF.Renderer()
	renderer.Write(doc, open(filename, "w"))
	
langs = {
	"cs-cz": ["Czech", "1029", "1250"],
	"nl-nl": ["Dutch", "1043", "1252"],
	"en-us": ["English", "1033", "1252"],
	"fr-fr": ["French", "1036", "1252"],
	"de-de": ["German", "1031", "1252"],
	"hu-hu": ["Hungarian", "1038", "1250"],
	"it-it": ["Italian", "1040", "1252"],
	"ja-jp": ["Japanese", "1041", "932"],
	"pl-pl": ["Polish", "1045", "1250"],
	"ru-ru": ["Russian", "1049", "1251"],
	"es-es": ["Spanish", "3082", "1252"],
	"uk-ua": ["Ukrainian", "1058", "1251"]
}

def invalid_language(lang):
	message = "Error: %s is an invalid language. "+\
		"Currently supported languages:\n" % lang
	for l in langs:
		message += "\t%s [%s]\n" % (langs[l][0], l)
	print >>sys.stderr, message
	sys.exit(-1)
	
def get_app_language(lang):
	if lang in langs:
		return langs[lang][1]
	return None
		
def get_app_codepage(lang):
	if lang in langs:
		return langs[lang][2]
	return None

def get_app_icon(builder):
	return (builder.appname+".exe").replace(" ", "_").replace("'", "").replace("?","").replace("-","_")

class Shortcut:
	@classmethod
	def create_start_menu_shortcut(cls, builder):
		return Shortcut("startMenu." + builder.options.manifest["appid"],
			"ProgramMenuDir", builder.appname, "INSTALLDIR",
			get_app_icon(builder), 0, True)
	
	@classmethod
	def create_desktop_shortcut(cls, builder):
		return Shortcut("desktop." + builder.options.manifest["appid"],
			"DesktopFolder", builder.appname, "INSTALLDIR",
			get_app_icon(builder), 0, True)
			
	# TODO: allow custom shortcuts
	def __init__(self, id, directory, name, working_dir, icon, icon_index=0, advertise=True):
		self.id = id
		self.directory = directory
		self.name = name
		self.working_dir = working_dir
		self.icon = icon
		self.icon_index = icon_index
		self.advertise = advertise
	
	def to_xml(self, indent):
		advertise = "yes"
		if not self.advertise: advertise = "no"
		
		return shortcut_template % {
			"indent": ("\t" * indent),
			"id": self.id,
			"directory": self.directory,
			"name": self.name,
			"working_dir": self.working_dir,
			"icon": self.icon,
			"icon_index": self.icon_index,
			"advertise": advertise}
	
class Directory:
	def __init__(self, relative_path, is_root=False):
		self.relative_path = relative_path
		self.name = os.path.basename(relative_path)
		self.id = get_file_id()
		self.files = []
		self.dirs = []
		self.is_root = is_root

	def escape_path(self, path):
		return path.replace('&', '&amp;')
			
	def add_file(self, relative_path, full_path, shortcuts=None):
		self.files.append({
			"guid": gen_guid(),
			"name": os.path.basename(relative_path),
			"relative_path": relative_path,
			"id": get_file_id(),
			"full_path": full_path,
			"shortcuts": shortcuts})

	def add_dir(self, dir):
		self.dirs.append(dir)

	def to_xml(self, indent=4):
		xml = ""
		if not self.is_root:
			xml += ("\t" * indent) + "<Directory Id=\"%s\" Name=\"%s\">\n" % \
				(self.id, self.name)

		for file in self.files:
			shortcuts_xml = ""
			if file["shortcuts"] is not None:
				for shortcut in file["shortcuts"]:
					shortcuts_xml += shortcut.to_xml(indent+1)
			
			xml += component_template % {
					"indent": ("\t" * indent),
					"id": file["id"],
					"filename": self.escape_path(file["name"]),
					"guid": file["guid"],
					"full_path": self.escape_path(file["full_path"]),
					"shortcuts": shortcuts_xml}
		
		for dir in self.dirs:
			xml += dir.to_xml(indent+1)
		
		if not self.is_root:
			xml += ("\t" * indent) + "</Directory>\n"
		return xml
	
	def to_components_xml(self, indent=4):
		xml = ""
		for file in self.files:
			xml += component_ref_template % {
					"indent": ("\t" * indent),
					"id": file["id"] }
		
		for dir in self.dirs:
			xml += dir.to_components_xml(indent)
		
		return xml

def run_command(args):
	subprocess.call(args)
	
def get_from_tiapp(tiapp, name, default_value):
	el = tiapp.find(name)
	if el is None or el is -1:
		return default_value
	return el.text
	
component_template = """
%(indent)s<Component Id="%(id)s" Guid="%(guid)s">
%(indent)s	<File Id="%(id)s" Source="%(full_path)s" KeyPath="yes">
%(shortcuts)s
%(indent)s	</File>
%(indent)s</Component>
"""

component_ref_template = """
%(indent)s<ComponentRef Id="%(id)s"/>
"""
shortcut_template = """
%(indent)s<Shortcut Id="%(id)s" Directory="%(directory)s" Name="%(name)s"
%(indent)s	WorkingDirectory="%(working_dir)s" Icon="%(icon)s"
%(indent)s	IconIndex="%(icon_index)s" Advertise="%(advertise)s" />
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
		if relative_path == "": file_relative_path = file
		
		if os.path.isfile(file_full_path):
			if file_relative_path == builder.appname + ".exe":
				shortcuts = []
				shortcuts.append(Shortcut.create_start_menu_shortcut(builder))
				shortcuts.append(Shortcut.create_desktop_shortcut(builder))
				current_dir.add_file(file_relative_path,
					file_full_path, shortcuts)
			else:
				current_dir.add_file(file_relative_path, file_full_path)
		else:
			newdir = Directory(file_relative_path)
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
	app_installer_template = read_template('app_installer_template.wxs')
	app_update_template = read_template('app_update_template.wxs')
		
	lang = get_from_tiapp(builder.options.tiapp, 'language', 'en-us')
	app_language = get_app_language(lang)
	app_codepage = get_app_codepage(lang)
	if app_language is None or app_codepage is None:
		invalid_language(lang)
	
	license_file = get_from_tiapp(builder.options.tiapp,
		'license', os.path.join(builder.options.appdir, 'LICENSE.txt'))
	license_rtf = default_license_rtf
	
	if os.path.exists(license_file):
		license_rtf = os.path.join(tempfile.gettempdir(),
			builder.appname + '_license.rtf')
		write_rtf(open(license_file, "r").read(), license_rtf)
	else:
		print >>sys.stderr, "Warning: No license file found, " + \
			"reverting to default Apache Public License v2"
	
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
	
	dialog_bmp = get_bmp('dialog-bmp', default_dialog_bmp)
	banner_bmp = get_bmp('banner-bmp', default_banner_bmp)
	
	titanium_installer_dll = os.path.join(builder.options.assets_dir, "titanium_installer.dll")
	crt_msm = os.path.join(builder.options.assets_dir, "Microsoft_VC80_CRT_x86.msm")
	
	app_version = builder.appversion
	version_parts = len(app_version.split("."))
	if version_parts < 3:
		app_version += ('.0' * (version_parts-1))

	app_guid = builder.options.manifest['guid'].upper()
	upgrade_guid = str(uuid.uuid5(uuid.UUID(app_guid), "upgrade")).upper()
	
	common_args = {
		"app_name": builder.appname,
		"app_exe": os.path.join(builder.base_dir, builder.appname + ".exe"),
		"app_icon": get_app_icon(builder),
		"app_id": builder.options.manifest['appid'],
		"app_guid": app_guid,
		"app_publisher": builder.options.manifest['publisher'],
		"app_description": builder.options.manifest['desc'],
		"app_version": app_version,
		"upgrade_guid": upgrade_guid,
		"program_menu_guid": gen_guid(),
		"app_language": app_language,
		"app_codepage": app_codepage,
		"license_rtf": license_rtf,
		"dialog_bmp": dialog_bmp,
		"banner_bmp": banner_bmp,
		"titanium_installer_dll": titanium_installer_dll,
		"crt_msm": crt_msm }
	
	update_args = common_args.copy()
	update_args["manifest_guid"] = gen_guid()
	update_args["manifest_path"] = os.path.join(builder.base_dir, "manifest")
	update_args["app_exe_guid"] = gen_guid()
	update_msi = build_msi(app_update_template, update_args,
		builder.appname, os.path.join(builder.base_dir, 'installer'))
	
	installed_file = os.path.join(builder.base_dir, ".installed")
	open(installed_file, "a").close() #touch
	
	root_dir = Directory(".", is_root=True)
	walk_dir(builder, builder.base_dir, root_dir)
	
	app_dirs_xml = root_dir.to_xml()
	components_xml = root_dir.to_components_xml()
	bundled_xml = ""
	#if builder.options.type != 'network':
	#	runtime_root_dir = Directory(".", is_root=True)
	#	walk_dir(builder, builder.options.runtime_dir, runtime_root_dir)
		
	#	runtime_files_xml = runtime_root_dir.to_xml()
	#	components_xml += runtime_root_dir.to_components_xml()
		
	#	module_dirs_xml = ""
	#	for module in builder.options.module_paths:
	#		module_root_dir = Directory(".", is_root=True)
	#		walk_dir(builder, module["path"], module_root_dir)
	#		module_files_xml = module_root_dir.to_xml()
	#		components_xml += module_root_dir.to_components_xml()
	#		module_dirs_xml += module_dir_template % {
	#			"module_id": module["name"],
	#			"module_version": module["version"],
	#			"module_files": module_files_xml,
	#			"indent": "\t\t"}
		
	#	bundled_xml = bundled_template % {
	#		"module_dirs": module_dirs_xml,
	#		"runtime_version": builder.options.runtime,
	#		"runtime_files": runtime_files_xml,
	#		"indent": "\t\t"}
	
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
	app_template_args["app_dirs"] = app_dirs_xml
	app_template_args["app_components"] = components_xml
	app_template_args["bundled_xml"] = bundled_xml
	app_template_args["dependencies"] = "&amp;".join(dependencies)
	app_template_args["bundled_modules"] = bundled_modules_xml
	app_template_args["bundled_runtime"] = bundled_runtime_xml
	
	install_msi = build_msi(app_installer_template, app_template_args,
		builder.appname, builder.options.destination)

	os.unlink(license_rtf)

	return install_msi
