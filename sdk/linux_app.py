import effess
import os
import os.path as p
from app import App

class LinuxApp(App):
	def stage(self, stage_dir, bundle):
		App.stage(self, stage_dir, bundle=bundle)

		contents = self.get_contents_dir()
		self.env.log(u'Copying kboot to %s' % contents)
		self.executable_path = p.join(self.contents, self.name)
		effess.copy(p.join(self.sdk_dir, 'kboot'), self.executable_path)

	def package(self, package_dir, bundle=False):
		longname = self.name + "-" + self.version
		def tar_callback(f, tar_file):
			print f
			tar_file.add(f, longname + "/" + f.replace(self.stage_dir + os.sep, ""))

		effess.make_tgz(self.stage_dir, p.join(package_dir, longname + '.tgz'),
			callback=tar_callback)
