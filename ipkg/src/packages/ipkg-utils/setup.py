import distutils.core

long_description = """
iPKG utilities implemented in Python

ipkg.py              implements Package class
ipkg-make-index.py   generates index for feed with only latest version of package listed
"""

distutils.core.setup( name = 'iPKG',
                      version = '0.1',
                      description = 'ipkg utilities implemented in python',
		      long_description = long_description,
                      author = 'Alexander Guy, Jamey Hicks',
                      author_email = 'a7r@andern.org',
		      licence = '??',
		      platforms = 'POSIX',
		      keywords = 'ipkg familiar',
                      url = 'http://www.handhelds.org/sources.html/',
                      py_modules = [ 'ipkg',  'ipkg-make-index' ],
		      scripts = ['ipkg-compare-indexes', 'ipkg-make-index', 'ipkg-update-index']
                      )
