#!/usr/local/bin/python

import sys, os, cgi, commands, time
import re
import posixfile
import ConfigParser
import ipkg
import smtplib
import libxml2

config = '/etc/packman.conf'


feeds_base = '/home/jamey/feeds'
feeds_base = '/home/ftp/feeds'
cgi.logfile = '/tmp/upload'
tmp_dir = '/tmp'
ipkgfind_url = 'http://ipkgfind.handhelds.org'

update_package_list = 0
cp = ConfigParser.ConfigParser()
cp.read(config)
try:
    if cp.has_option('config', 'feeds_base'):
        feeds_base = cp.get('config', 'feeds_base')
    if cp.has_option('config', 'logfile'):
        cgi.logfile = cp.get('config', 'logfile')
    if cp.has_option('config', 'tmp_dir'):
        tmp_dir = cp.get('config', 'tmp_dir')
    if cp.has_option('config', 'update_index'):
        update_package_list = cp.get('config', 'update_index')
except:
    pass
cp = None

logprefix = time.strftime('%c') + ' -- '
try:
     for k in ['REMOTE_ADDR']:
          if (os.environ.has_key(k)):
               logprefix = logprefix + ('%s' % (os.environ[k],))
except:
     pass

def log(string):
     cgi.log(logprefix + ': ' + string)

def filename_is_valid(filename):
     if re.match(r'^[+-._A-Za-z0-9]+$', filename): return 1
     return 0

def copy(srcfilename, dstfilename):
    srcfile = open(srcfilename, 'r')
    dstfile = open(dstfilename, 'w')
    if not srcfile:
	return 1	
    if not dstfile:
	return 1	
    while (1):
        str = srcfile.read(512)
        if (str):
            nbytes = dstfile.write(str)
        else:
            break
    dstfile.close()
    srcfile.close()
    return 0	

def make_index(feed_dir):
    os.chdir(feed_dir)
    f = posixfile.open(tmp_dir + "/Packages.lck", 'w')
    f.lock('w|')
    (rc, outtext) = commands.getstatusoutput('/usr/local/bin/ipkg-make-index . > /tmp/Packages')
    if (rc != 0):
        print ('<b>Failed to create Packages file with error=%s and output=%s</b>' % (rc,outtext)) 
    else:
        commands.getstatusoutput('mv /tmp/Packages Packages')
    f.lock('u')
    f.close()
    print '<b>Updated Packages</b>'	
    return

def update_index(feed_dir, pkgfilename):
    f = posixfile.open(tmp_dir + "/Packages.lck", 'w')
    f.lock('w|')
    (rc, outtext) = commands.getstatusoutput('ipkg-update-index %s %s' % (feed_dir, pkgfilename) )
    if (rc != 0):
        print ('<b>Failed to update Packages file with error=%s and output=%s</b>' % (rc,outtext)) 
    f.lock('u')
    f.close()
    print ('<b>added %s to Packages</b>' % (pkgfilename,))
    return

def announce_to_cia(uploader, feedname, filename, changenotice=None):
    msg = "From: commits@handhelds.org\r\nTo: commits@picogui.org\r\nContent-Type: text/plain;\r\nSubject: SendToChannels handhelds.org\r\n\r\n{light blue}" + filename +"{normal} uploaded to {light green}" + feedname + "{normal} by {orange}" + uploader + "{normal}\r\n"
    mailmsg = "From: " + uploader + "\r\nTo: familiar-updates@handhelds.org\r\nContent-Type: text/plain;\r\nSubject: " + filename + " uploaded to " + feedname + "\r\n\r\n\r\n"
    if changenotice:
       mailmsg = mailmsg + changenotice + "\r\n"
    server = smtplib.SMTP('localhost')
    server.sendmail('commits@handhelds.org', 'commits@picogui.org', msg)
    server.sendmail(uploader, 'familiar-updates@handhelds.org', mailmsg)
    server.quit()

def fixup_filename(filename):
    if filename and filename[:8] == "C:\\Temp\\":
	filename = filename[8:] 	
    return filename

def update_feed_rss(uploader, feedname, filename, upload_time):
    rss_filename = feeds_base + '/' + feedname + '/upload.rss'
    m = re.match(r'([^_]+)_([^_]+)_([^_]+).ipk', filename)
    if not m:
        return
    (pkg_name, pkg_version, pkg_arch) = m.groups()
    doc = libxml2.parseFile(rss_filename)
    items = []
    c = doc.children.children
    while c:
        if c.name == 'item':
	    items.append(c)
        c = c.next
    oldest = items[len(items)-1]
    items_parent = items[0].parent
    c = oldest.children
    while c:
        if c.name == 'title':
            c.children.setContent('%s (version %s arch %s by %s on %s)' % (pkg_name, pkg_version, pkg_arch, uploader, upload_time))
        elif c.name == 'link':
            c.children.setContent(ipkgfind_url + '/details.phtml?package=' + pkg_name)
        else:
            pass
        c = c.next
    # now move c from end of list to head of list
    oldest.unlinkNode()
    items_parent.children.addPrevSibling(oldest)
    f = open(rss_filename, 'w')
    doc.dump(f)

def handle_upload(feeditem, fileitem, sigfileitem, srcfileitem, changenotice=None):
     global logprefix
     global update_package_list	
     
     feedname    = feeditem.value
     filename    = fileitem.filename
     sigfilename = sigfileitem.filename
     srcfilename = srcfileitem.filename
     filename = fixup_filename(filename)
     sigfilename = fixup_filename(sigfilename)
     srcfilename = fixup_filename(srcfilename)
     # print "filename=%s\n" % (filename,)
     # print "sigfilename=%s\n" % (sigfilename,)
     # print "srcfilename=%s\n" % (srcfilename,)

     if not filename_is_valid(feedname):
          print ('<b>%s</b> is an invalid filename\n' % (feedname,))
          log('invalid feedname %s' % (feedname,))
          return 0
     if not filename_is_valid(filename):
          print ('<b>%s</b> is an invalid filename\n' % (filename,))
          log('invalid filename %s' % (filename,))
          return 0
     if not filename_is_valid(sigfilename):
          print ('<b>%s</b> is an invalid filename\n' % (sigfilename,))
          log('invalid sigfilename %s' % (sigfilename,))
          return 0
     if srcfilename and not filename_is_valid(srcfilename):
          print ('<b>%s</b> is an invalid filename\n' % (srcfilename,))
	  log('invalid srcfilename %s' % (srcfilename,))
	  return 0
     if (filename == 'keyring.gpg' or sigfilename == 'keyring.gpg'):
         print '<b>not allowed to overwrite keyring</b>'
         return 0

     feed_dir = feeds_base + '/' + feedname + '/'
     tmp_dir = "/tmp/"
     file = open(tmp_dir + filename, 'w')
     file.write(fileitem.value)
     file.close()
     sigfile = open(tmp_dir + sigfilename, 'w')
     sigfile.write(sigfileitem.value)
     sigfile.close()
     
     ipk = ipkg.Package(tmp_dir + filename)
     if not ipk:
          print '<p>Unable to parse ' + filename + '</p>'
          return 0
     if not srcfilename and not ipk.source and ipk.architecture != "all":
          print '<p><b>Error:</b> ' + filename + ' lacks a Source field in its control file.</p>'
          return 0
     if not ipk.isdeb:
	  print '<p><b>Error:</b> ' + filename + ' is an old-format archive.  Please upgrade ipkg-build.</p>'
          return 0
     if ipk.filename_header:
          print '<p><b>Error:</b> ' + filename + ' has a Filename header in its control file.</p>'
          return 0

     keyringfile = feed_dir + 'keyring.gpg'
     try:
          (exitstatus, outtext) = commands.getstatusoutput("/usr/local/bin/gpgv --keyring %s %s" % (keyringfile, tmp_dir + sigfilename))
     except:
          cgi.log('caught an exception')
          return 0
     print '<pre>' + outtext + '</pre>'
     if (exitstatus != 0):
          log('/usr/local/bin/gpgv %s %s exitstatus=%d' % (filename, sigfilename, exitstatus,)) 
          log('  ' + outtext)
     rc = exitstatus
     if (rc != 0):
         return 0
     
     match = re.search("Good signature from.*\<(.+)\>", outtext)
     if (match):
         uploader = match.group(1)
         announce_to_cia(uploader, feedname, filename)
         update_feed_rss(uploader, feedname, filename, time.strftime('%c'))

     rc = copy(tmp_dir + filename, feed_dir + filename)
     if (rc != 0):
         print ('<p>error %s copying %s to %s' % (rc, tmp_dir + filename, feed_dir + filename))
     os.unlink(tmp_dir + filename)
         
     rc = copy(tmp_dir + sigfilename, feed_dir + sigfilename)
     if (rc != 0):
         print ('<p>error %s copying %s to %s' % (rc, tmp_dir + sigfilename, feed_dir + sigfilename))
     os.unlink(tmp_dir + sigfilename)
     cgi.logfile = feed_dir + 'upload.log'	
     log('uploaded %s the package %s %s' % (feedname, filename, sigfilename))
     if changenotice:
         cgi.log('\t' + changenotice)
     if srcfilename:
         log('srcfile=' + srcfilename)
         overridefile = open(feed_dir + filename + ".override", 'w')
         overridefile.write('Source: ' + srcfilename + '\n')
         overridefile.close()
         srcfile= open(tmp_dir + srcfilename, 'w')
         srcfile.write(srcfileitem.value)
         srcfile.close()
         log('srcfile written to /tmp')

         rc = copy(tmp_dir + srcfilename, feed_dir + srcfilename)
         log('copied srcfile to feed, rc=%s' % (rc,))
         if (rc != 0):
             print ('<p>error %s copying %s to %s' % (rc, tmp_dir + sigfilename, feed_dir + sigfilename))
         os.unlink(tmp_dir + srcfilename)
         log('uploaded %s the source file %s' % (feedname, srcfilename))

     print "<p>Packages file now updated every 10 minutes by cron job</p>\n"
     # remove the tmp files
     return 1

def main():
    global update_package_list
    print "Content-type: text/html\n"
    form = cgi.FieldStorage()
    print '<html>'
    print '<head><title>Handhelds.Org Package Manager -- upload</title></head>'
    print '<body><h1>Handhelds.Org Package Manager -- upload</h1>'
    if form and form.has_key("filename") and form.has_key("signaturefilename") and form.has_key("sourcefilename"):
	 print '<ul>'
	 print '<li>'
	 print form["feedname"].value
	 print '<li>'
	 print form["filename"].filename
	 print '<li>'
         print form["signaturefilename"].filename
	 print '<li>'
         print form["sourcefilename"].filename
         print '</ul>'
         if form.has_key('update_index'):
             update_package_list = form["update_index"]
	 if form.has_key('changenotice'):
	     changenotice= form['changenotice']
	 else:
	     changenotice=None
         if handle_upload(form["feedname"], form["filename"], form["signaturefilename"],form["sourcefilename"], changenotice):
              print '<p><b>Upload completed successfully</b>'
              print ('<p><a href="/feeds/%s/Packages">Updated Packages File</a>' % (form["feedname"].value, ))
         else:
              print '<p><b>Upload failed</b>'
    elif form and form.has_key("feedname") and form.has_key("update_index"):
       # feed_dir = feeds_base + '/' + form["feedname"].value + '/'
       # make_index(feed_dir)
       print "Packages file now updated hourly by cron job at 11 after the hour\n"
    else:
       print """
       <h1>Signing Packages</h1>
       The following command will generate the file foo.ipk.asc, containing
       an ASCII signature of the package foo.ipk:
       <pre>
           gpg -sb --armor foo.ipk
       </pre>
       <p>

       See <a
       href="/pipermail/familiar/2001-December/003955.html">Jamey's
       announcement of this package upload utility</a> for some more
       details.

       <h1>Package to Upload</h1>
       <form action="/cgi-bin/upload-package.cgi" method="POST" enctype="multipart/form-data">
       <table>
       <tr><td>Feed Name <td><select name="feedname">
          <option selected>unstable</option>
          <option>devel</option>
       </select></tr>
       <tr><td>Filename <td><input type="file" name="filename"></tr>
       <tr><td>OpenPGP Signature Filename <td> <input type="file" name="signaturefilename"></tr>
       <tr><td>Sources Filename - Optional <td> <input type="file" name="sourcefilename"></tr>
       <tr><td>Change Notice - Optional <td> <input type="file" name="changenotice"></tr>
       </table>
       <p>Please make sure that your package's
       control file has a Source field.
       <p>The Source field is intended to allow automated retrieval of the source
       package from which your package was built.  If you are uploading a source
       package along with your binary package, the Source field should contain
       the name of the source package.  Otherwise, it should contain one of the
       following:
       <ul>
       <li>a single URL pointing to a source tarball
       <li>multiple URLs pointing to the multiple files (.dsc, .orig.tar.gz, .diff.gz)
       comprising a Debian source package
       </ul>
       <input type="submit">
       </form>
"""
    print '</body>'
    print '</html>'

main()


