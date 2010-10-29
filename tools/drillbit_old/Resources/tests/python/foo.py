from bar import Bar
from guido_rocks import Google

class Foo(object):
	
	def bar(self):
		return 'hello,world'
	
	def yum(self):
	    bar = Bar()
	    return bar.mitzvah()
	
	def what_is_love(self,i_am):
	    bar = Bar()
	    return bar.give_me_some(i_am)

	def go_google(self):
		return Google().mobile()
		