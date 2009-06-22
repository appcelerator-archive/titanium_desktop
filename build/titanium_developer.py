# quick script to automate build/run of titanium developer
import os, sys, platform, shutil, distutils.dir_util as dir_util
import titanium_version, subprocess

cwd = os.path.dirname(os.path.abspath(__file__))
os_map = { 'Windows': 'win32', 'Darwin': 'osx', 'Linux': 'linux' }
os_name = os_map[platform.system()]
developer_path = os.path.abspath(os.path.join(cwd, '..', '..', 'titanium_developer'))
build_path = os.path.abspath(os.path.join(cwd, 'titanium_developer'))

if os.path.exists(build_path):
	dir_util.remove_tree(build_path)
os.makedirs(build_path)

def run(args):
	return subprocess.Popen(args, stdout=subprocess.PIPE).communicate()[0]

def get_sdk():
	if os_name == 'win32':
		return os.path.join('C:\\', 'ProgramData', 'Titanium')
	elif os_name == 'osx':
		return os.path.join(os.path.expanduser('~'), 'Library', 'Application Support', 'Titanium')
	elif os_name == 'linux':
		# ??
		return os.path.join('')

sdk_path = get_sdk()
support_path = os.path.join(sdk_path, 'sdk', os_name, titanium_version.version)
tibuild_path = os.path.join(support_path, 'tibuild.py')

run([sys.executable, tibuild_path, '-d', build_path, '-s', sdk_path, developer_path, '-a', support_path, '-r'])
