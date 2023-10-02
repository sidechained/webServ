#!/usr/bin/python3

import os
import cgi

print("HTTP/1.1 200 OK")
print("Content-type: text/html\r\n\r\n")

print("<html>")
print("<head>")
print("<h2>Environment:</h2><br>")


print("<html>")
print("<body>")
for param in os.environ.keys():
    print("<b>%20s</b>: %s<br>" % (param, os.environ[param]))

print("</body>")
print("</html>")

#!/usr/bin/python3

#import cgi, os

form = cgi.FieldStorage()

# Get filename here
fileitem = form['filename']

# Test if the file was uploaded
if fileitem.filename:
   open(os.getcwd() + '/cgi-bin/tmp/' + os.path.basename(fileitem.filename), 'wb').write(fileitem.file.read())
   message = 'The file "' + os.path.basename(fileitem.filename) + '" was uploaded to ' + os.getcwd() + '/cgi-bin/tmp'
else:
   message = 'Uploading Failed'

print("Content-Type: text/html;charset=utf-8")
print ("Content-type:text/html\r\n")
print("<H1> " + message + " </H1>")