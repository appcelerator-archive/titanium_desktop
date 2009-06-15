#!/usr/bin/env python
#
# this script creates build/<os>/titanium_testapp folder
# after running this script, you can execute the test app with <os>/titanium_testapp/titanium_testapp

import os, os.path as path, shutil, glob
import distutils.dir_util as dir_util
import sys

app_name = 'titanium_testapp'
version = build.version

runtime_libs = ['kroll', 'khost']
third_party = ['webkit', 'poco']
module_dirs = [
    'api', 'javascript', 'foo', 'ruby', 'python',
    'ti.Platform', 'ti.App', 'ti.UI', 'tinetwork',
    'ti.Growl', 'ti.Filesystem', 'ti.Media', 'ti.Desktop', 'ti.Process',
    'ti.Notification', 'ti.Monkey', 'ti.Analytics'
]

def moduleID(dir):
    id = dir.lower()
    id = id.replace('.', '')
    return id

modules = build.modules
for name in modules:
    mid = moduleID(name)
    modules[mid] = name

lib_prefix = ''

if sys.platform == 'win32':
	osname = 'win32'
	lib_ext = '.dll'
	exe_ext = '.exe'
	lib_dir = 'bin'
elif sys.platform == 'linux2':
	osname = 'linux'
	lib_ext = '.so'
	exe_ext = ''
	lib_prefix = 'lib'
	lib_dir = 'lib'
	third_party.append('libcurl')
	third_party.append('libicu')
elif sys.platform == 'darwin':
    osname = 'osx'
    lib_ext = '.dylib'
    exe_ext = ''
    lib_prefix = 'lib'
    lib_dir = 'lib'

top_dir = path.abspath('./../')
build_dir = path.join(top_dir, 'build')
build_dir = path.join(build_dir, osname)

support_dir = path.join(top_dir,'support',osname)

app_dir = path.join(build_dir, app_name)
if osname is 'osx':
    app_dir = path.join(build_dir, app_name+'.app', 'Contents')
    
runtime_dir = path.join(app_dir, 'runtime');
modules_dir = path.join(app_dir, 'modules');


# delete current package if one is there
if path.isdir(app_dir):
    dir_util.remove_tree(app_dir)

# create directories for app
for d in [app_dir, runtime_dir, modules_dir]:
    os.makedirs(d)
    
# Gather all runtime third-party libraries
for lib in runtime_libs:
    f = path.join(build_dir, lib_prefix + lib + lib_ext)
    shutil.copy(f, runtime_dir)

for tp in third_party:
    if osname is 'osx':
        pattern = path.join(build.third_party, tp, '*.framework')
        for d in glob.glob(pattern):
            dest = path.join(runtime_dir, path.basename(d))
            shutil.copytree(d, dest, symlinks=True)

    lib_files_dir = path.join(build.third_party, tp, lib_dir)
    if path.exists(lib_files_dir):
    	dir_util.copy_tree(lib_files_dir, runtime_dir, preserve_symlinks=1)

# Gather all module libs
for m in modules.keys():
    module_dir = path.join(top_dir, 'modules', modules[m])
    if not path.exists(module_dir):
        module_dir = path.join(top_dir, 'kroll', 'modules', modules[m])
    
    mlib = lib_prefix + m + 'module' + lib_ext
    mlib = path.join(build_dir, mlib)
    ## we'll have to make this pluggable at some point
    if not path.exists(mlib):
        mlib = path.join(module_dir, '%smodule.js' % m)
    if not path.exists(mlib):
        mlib = path.join(module_dir, '%smodule.py' % m)
    if not path.exists(mlib):
        mlib = path.join(module_dir, '%smodule.rb' % m)
    
    out_dir = '%s/%s' % (modules_dir, m)
    os.makedirs(out_dir)
    
    if path.exists(mlib):
        shutil.copy(mlib, out_dir)
    
    app_resources = path.join(module_dir, 'AppResources')
    if path.exists(app_resources):
        dir_util.copy_tree(app_resources, path.join(modules_dir, m, 'AppResources'))
    
# create executable file
kboot_file = path.join(build_dir, 'kboot' + exe_ext)
exe_dir = app_dir
if osname is 'osx':
    exe_dir = path.join(app_dir, 'MacOS')
    os.makedirs(exe_dir)
exe_path = path.join(exe_dir, app_name + exe_ext)
shutil.copy(kboot_file, exe_path)

if osname is 'osx':
    plist = open(path.join('..', 'support', 'osx', 'Info.plist')).read()
    plist = plist.replace('APPEXE', 'titanium_testapp')
    plist = plist.replace('APPNAME', 'Titanium Test App')
    plist = plist.replace('APPICON', 'titanium.icns')
    plist = plist.replace('APPID', 'com.titaniumapp.testapp')
    plist = plist.replace('APPNIB', 'MainMenu')
    plist = plist.replace('APPVER', '0.1')
    out_file = open(path.join(app_dir, 'Info.plist'), 'w')
    out_file.write(plist)
    out_file.close()
    menu_nib = path.join(build_dir, 'modules', 'ti.UI', 'MainMenu.nib')
    lproj = path.join(app_dir, 'Resources', 'English.lproj')
    os.makedirs(lproj)
    shutil.copy(menu_nib, lproj)
    shutil.copy(path.join(support_dir,'titanium.icns'),path.join(app_dir,'Resources'))

    
# copy test app resources
app_src = path.join(top_dir, 'apps')
app_src = path.join(app_src, 'testapp')
dir_util.copy_tree(app_src, app_dir, preserve_symlinks=True)

# copy UI resources
ui_js = path.join(build_dir, 'modules', 'tiui', 'Resources', 'all', 'ui.js')
tiui_module_dir = path.join(app_dir, 'modules', 'tiui')
shutil.copy(ui_js, tiui_module_dir)

if len(sys.argv) > 1 and sys.argv[1] == '--run':
    print "exec: " + exe_path
    os.system(exe_path+" --debug")
