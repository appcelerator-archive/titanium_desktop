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
		testcoverage['modules'][module]['entries'] = {}
		for entry in apicoverage[module].values():
			entries += 1
			module_entries += 1
			testcoverage['modules'][module]['entries'][entry['name']] = 0
			
		testcoverage['modules'][module]['coverage'] = 0
		testcoverage['modules'][module]['total'] = module_entries
		
	testcoverage['total'] = entries
	coverage = 0
	for dir in dirs:
		files = set()
		for f in GlobDirectoryWalker(dir, patterns=['*.prof']): files.add(f)
		for file in files:
			contents = open(file,'r').read()
			for line in contents.splitlines():
				tokens = line.split(',')
				if tokens[1] in ["call", "get"]:
					entry = tokens[2]
					entry = entry.replace('Titanium.','')
					module = '<global>'
					if entry.find(".") != -1: module = entry[0:entry.find(".")]
					if entry.find('.') == -1: continue
					entry = entry[entry.find('.')+1:]
					
					print 'found ' + tokens[1] + ' for module: ' + module + ' , API: '+entry
				
					if module not in testcoverage['modules']:
						# this API wasn't documented, skip for now
						continue
					if entry not in testcoverage['modules'][module]['entries'] or testcoverage['modules'][module]['entries'][entry] == 0 :
						testcoverage['modules'][module]['entries'][entry] = 1
						# only count each entry once in the global/module coverage count
						coverage += 1
						testcoverage['modules'][module]['coverage'] += 1
					else:
						testcoverage['modules'][module]['entries'][entry] += 1

	testcoverage['coverage'] = coverage
	outfile.write(simplejson.dumps(testcoverage, sort_keys=True, indent=4))

if __name__ == '__main__':
	if len(sys.argv)!=4:
		print "Usage: %s <dir> <apicoverage> <outfile>" % os.path.basename(sys.argv[0])
		sys.exit(1)
	f = open(os.path.expanduser(sys.argv[3]), 'w')
	dirs = []
	dirs.append(os.path.abspath(os.path.expanduser(sys.argv[1])))
	apicoverage = simplejson.loads(open(os.path.expanduser(sys.argv[2]), 'r').read())
	generate_test_coverage(dirs,apicoverage,f)
