import uuid
import simplejson as json
import os

completions_template = """<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>name</key>
	<string>Completions</string>
	<key>scope</key>
	<string>source.js.titanium</string>
	<key>settings</key>
	<dict>
		<key>completions</key>
		<array>
			%s
		</array>
	</dict>
	<key>uuid</key>
	<string>3102AC7B-6A7B-4972-9318-C96432FD3075</string>
</dict>
</plist>
"""

template = """<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>content</key>
	<string>%s</string>
	<key>name</key>
	<string>%s</string>
	<key>scope</key>
	<string>source.js.titanium</string>
	<key>tabTrigger</key>
	<string>%s</string>
	<key>uuid</key>
	<string>%s</string>
</dict>
</plist>
"""

info_plist = """<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>name</key>
	<string>Titanium</string>
	<key>mainMenu</key>
	<dict>
		<key>items</key>
		<array>
			%s
		</array>
	</dict>
	<key>ordering</key>
	<array></array>
	<key>uuid</key>
	<string>DE60F162-924B-4550-B29A-67DAE30EC52F</string>
</dict>
</plist>
"""

syntaxes = """<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>comment</key>
	<string>Titanium</string>
	<key>foldingStartMarker</key>
	<string>(^.*{[^}]*$|^.*\([^\)]*$|^.*/\*(?!.*\*/).*$)</string>
	<key>foldingStopMarker</key>
	<string>(^\s*\}|^\s*\)|^(?!.*/\*).*\*/)</string>
	<key>keyEquivalent</key>
	<string>^~T</string>
	<key>name</key>
	<string>Titanium</string>
	<key>patterns</key>
	<array>
		<dict>
			<key>match</key>
			<string>\b(Titanium)\b</string>
			<key>name</key>
			<string>support.class.js.titanium</string>
		</dict>
		<dict>
			<key>begin</key>
			<string>(\$)(\()</string>
			<key>beginCaptures</key>
			<dict>
				<key>1</key>
				<dict>
					<key>name</key>
					<string>support.class.js.titanium</string>
				</dict>
				<key>2</key>
				<dict>
					<key>name</key>
					<string>punctuation.section.class.js</string>
				</dict>
			</dict>
			<key>contentName</key>
			<string>meta.selector.titanium</string>
			<key>end</key>
			<string>(\))</string>
			<key>endCaptures</key>
			<dict>
				<key>1</key>
				<dict>
					<key>name</key>
					<string>punctuation.section.class.js</string>
				</dict>
			</dict>
			<key>patterns</key>
			<array>
				<dict>
					<key>include</key>
					<string>source.js</string>
				</dict>
			</array>
		</dict>
		<dict>
			<key>match</key>
			<string>(Titanium)\.</string>
			<key>name</key>
			<string>support.function.js.titanium</string>
		</dict>
		<dict>
			<key>include</key>
			<string>source.js</string>
		</dict>
	</array>
	<key>scopeName</key>
	<string>source.js.titanium</string>
	<key>uuid</key>
	<string>101C29E1-B91C-4CC5-9578-F1F0C5A0642B</string>
</dict>
</plist>
"""


def make_template(module,fn,tab,out):
	id = uuid.uuid4().hex
	out.write(template % (module,fn,tab,id))
	return id

#make_template('Titanium.Filesystem','Filesystem')

top_level = []
all_methods = []
apis = {}
sub_methods = {}

def generate_textmate_bundle(json_file,outdir):

	for dir in ['Commands','Preferences','Snippets','Syntaxes']:
		if not os.path.exists(os.path.join(outdir,dir)):
			os.makedirs(os.path.join(outdir,dir))

	pref_dir = os.path.join(outdir,'Preferences')
	snip_dir = os.path.join(outdir,'Snippets')
	syntax_dir = os.path.join(outdir,'Syntaxes')
	
	data = json.loads(json_file.read())
	
	completions=[]
	completion_menus = {}

	for namespace in data:
		sf = open(os.path.join(snip_dir,namespace+'.tmSnippet'),'w+')
		theid = make_template('Titanium.'+namespace,namespace,'Titanium',sf)
		sf.close()
		completions.append("Titanum.%s" % namespace)
		completion_menus[namespace] = theid

		methods = data[namespace]
		for method in data[namespace]:
			entry = data[namespace][method]
			method_name = namespace+'.'+method
			try:
				all_methods.index(method_name)
			except:
				all_methods.append(method_name)

			
	for method in sorted(all_methods):
		path = ''
		tokens = method.split('.')
		cur = ''
		for i in range(0,len(tokens)):
			if i > 0:
				cur = path
			path+=tokens[i]
			try:
				top_level.index(path)
			except:
				top_level.append(path)
				if i+1 == len(tokens):
					method_name = path.replace(cur,'')
					package_name = cur[0:-1]
					name = path.replace('.','_')
					sf = open(os.path.join(snip_dir,name+'.tmSnippet'),'w+')
					theid = make_template('Titanium.'+path,method_name,'Titanium.'+package_name,sf)
					sf.close()
					completions.append("Titanium.%s" % path)
					sub_methods["Titanium.%s" % path]=theid
			if i+1 < len(tokens):
				path+='.'
	
	completions = sorted(completions)
	cstr = ''
	for i in completions:
		cstr+="<string>%s</string>\n" % i

	cf = open(os.path.join(pref_dir,'Completions.tmPreferences'),'w+')
	cf.write(completions_template % cstr)
	cf.close()
	
	cf = open(os.path.join(syntax_dir,'Titanium.tmLanguage'),'w+')
	cf.write(syntaxes)
	cf.close()
	
	plist = ''
	dicts = []
	for key in sorted(completion_menus):
		value = completion_menus[key]
		plist+="<string>%s</string>\n" % value
	#	dicts.append("<key>%s</key>\n<dict>\n<key>items</key>\n")


		# <key>submenus</key>
		# <dict>
		# 	<key>0E442AAB-1430-4A69-B895-737F7B4391CF</key>
		# 	<dict>
		# 		<key>items</key>
		# 		<array>
		# 			<string>5D82DAB0-F208-427F-95F3-C3BB75C0E548</string>
		# 			<string>AD0CBFB3-D060-4765-A1EA-DBDB88D80171</string>
		# 			<string>B3109B1B-C7F2-4CFC-A49A-FB5BD7D79CFF</string>
		# 			<string>E3AAED4C-3799-4B93-AA8F-E3326CC7F91C</string>
		# 			<string>47EC0402-E273-472C-BA6A-20C02AB6313D</string>
		# 			<string>9C6361E2-2948-4EE7-8088-054A690FF382</string>
		# 		</array>
		# 		<key>name</key>
		# 		<string>AJAX</string>
		# 	</dict>
	
	f = open(os.path.join(outdir,'info.plist'),'w+')
	f.write((info_plist % plist))
	f.close()

	