#!/usr/bin/env python
import os, sys, simplejson, platform
platforms = { 'Darwin': 'osx', 'Windows': 'win32', 'Linux': 'linux' }
cwd = os.path.abspath(os.path.dirname(__file__))
json = os.path.join(cwd, '..', 'build', platforms[platform.system()], 'testcoverage.json')

if not os.path.exists(json):
	print >>sys.stderr, "Error: testcoverage.json doesn't exist. Please run drillbit with all tests, and then run scons testcoverage"
	exit(-1)

c = simplejson.loads(open(json,'r').read())

coverage = c['coverage']
total = c['total']
p = ((coverage/float(total)) * 100)
print 'Total Coverage: %d of %d (%d%%) API' % (coverage,total,p)
print 'Module Coverage'
for module in c['modules'].keys():
	m = c['modules'][module]
	coverage = m['coverage']
	total = m['total']
	p = ((coverage/float(total)) * 100)
	print '  %s: %d of %d (%d%%) API' % (module,coverage,total,p)

