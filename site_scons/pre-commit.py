import sys,re,os
import os.path as path
fname_regex = re.compile('diff --git a/(.*) b/')
not_a_line_regex = re.compile('^-')

fname = None
bad_files = []
for line in sys.stdin:
	if not_a_line_regex.match(line):
		continue

	if fname and (not fname in bad_files) and line.find('\r') != -1:
		found_bad_newlines = True
		bad_files.append(fname)

	# detect next filename
	m = fname_regex.match(line)
	if m:
		fname = m.group(1)
		found_bad_newline = False

if len(bad_files) > 0:
	print 'Found Windows-style newlines in the following files:'
	for file in bad_files:
		print '\t%s' % file
	paths = ['\'' + path.join(os.getcwd(), f) + '\'' for f in bad_files]
	paths = ' '.join(paths)
	print ''
	print 'You can fix these files by running:'
	print ' $ python %s %s' % (path.join(os.getcwd(), 'site_scons', 'crlf.py'), paths)
	print ''
	print 'If you REALLY want to make this commit you can override with --no-verify'
	print 'It\'s probably not a good idea though -- think of the repositories!'
	sys.exit(666)
else:
	sys.exit(0)
