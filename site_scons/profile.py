#!/usr/bin/env python
#
# profiler examiner
#
import os, sys, re
import os.path as path
import fnmatch, os, sys
import simplejson as json

class GlobDirectoryWalker:
	# a forward iterator that traverses a directory tree

	def __init__(self, directory, pattern="*"):
		self.stack = [directory]
		self.pattern = pattern
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
				if fnmatch.fnmatch(file, self.pattern):
					return fullname


class Profile(object):
	
	def __init__(self,file):
		self.file = file
		self.total_time = 0
		self.by_api= {}
		for line in open(file).readlines():
			tokens = line.strip().split(",")
			offset = tokens[0]
			op = tokens[1]
			api = tokens[2]
			duration = int(tokens[3])
			self.total_time += duration
			try:
				entry = self.by_api[api]
				entry['count'] += 1
				entry['duration'] += duration
			except:
				self.by_api[api] = {'count':1,'duration':duration}

		for api in sorted(self.by_api):
			print api + " => " + str(self.by_api[api])		
		
class TestProfile(object):

	def __init__(self,dir):
		self.dir = dir
		if not hasattr(os, 'uname') or self.matches('CYGWIN'):
			self.osname = 'win32'
		elif self.matches('Darwin'):
			self.osname = 'osx'
		elif self.matches('Linux'):
			self.osname = 'linux'

	def matches(self,n): return bool(re.match(os.uname()[0], n))
	
	def examine(self):
		for file in GlobDirectoryWalker(self.dir,"*.prof"):
			print file
			Profile(file)


if "__main__" == __name__:
	if len(sys.argv) < 2:
		print "Usage: %s <test_results_dir>" % os.path.basename(sys.argv[0])
		sys.exit(1)
		
	p = TestProfile(sys.argv[1])
	p.examine()

