#
# Titanium Test Coverage Generator
#
import os, sys, fnmatch, re
import simplejson

class GlobDirectoryWalker:
	# a forward iterator that traverses a directory tree
	def __init__(self, directory, patterns=['*']):
		self.stack = [directory]
		self.patterns = patterns
		self.files = []
		self.index = 0

	def __getitem__(self, index):
		while 1:
			try:
				file = self.files[self.index]
				self.index = self.index + 1
			except IndexError:
				# pop next directory from stack
				self.directory = self.stack.pop()
				self.files = os.listdir(self.directory)
				self.index = 0
			else:
				# got a filename
				fullname = os.path.join(self.directory, file)
				if os.path.isdir(fullname) and not os.path.islink(fullname):
					self.stack.append(fullname)
				for pattern in self.patterns:
					if fnmatch.fnmatch(file, pattern):
						return fullname

def generate_test_coverage(dirs, apicoverage, outfile):
	api_regex = 'Titanium\.([A-Za-z0-9_]+)(\.([A-Za-z0-0_]+))?'
	
	testcoverage = {}
	testcoverage['modules'] = {}
	
	entries = 0
	for module in apicoverage.keys():
		module_entries = 0
		testcoverage['modules'][module] = {}
		for entry in apicoverage[module]:
			entries += 1
			module_entries += 1
		testcoverage['modules'][module]['coverage'] = 0
		testcoverage['modules'][module]['total'] = module_entries
		testcoverage['modules'][module]['entries'] = {}
		
	testcoverage['total'] = entries
	coverage = 0
	for dir in dirs:
		files = set()
		for f in GlobDirectoryWalker(dir, patterns=['*.js']): files.add(f)
		for file in files:
			contents = open(file,'r').read()
			for match in re.finditer(api_regex, contents):
				module = match.group(1)
				entry = match.group(3)
				if entry is not None:
					# ignore Titanium.<entry> for now.. they're not documented yet?
					#entry = module
					#module = 'Titanium'
					if entry not in testcoverage['modules'][module]['entries']:
						testcoverage['modules'][module]['entries'][entry] = 1
						# only count each entry once in the global/module coverage count
						coverage += 1
						testcoverage['modules'][module]['coverage'] += 1
					else:
						testcoverage['modules'][module]['entries'][entry] += 1

	testcoverage['coverage'] = coverage
	outfile.write(simplejson.dumps(testcoverage, sort_keys=False, indent=4))

if __name__ == '__main__':
	if len(sys.argv)!=4:
		print "Usage: %s <dir> <apicoverage> <outfile>" % os.path.basename(sys.argv[0])
		sys.exit(1)
	f = open(os.path.expanduser(sys.argv[3]), 'w')
	dirs = []
	dirs.append(os.path.abspath(os.path.expanduser(sys.argv[1])))
	apicoverage = simplejson.loads(open(os.path.expanduser(sys.argv[2]), 'r').read())
	generate_test_coverage(dirs,apicoverage,f)