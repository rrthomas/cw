#!/cw
# this is simply a mimic of the test.cgi that (tends to)
# come with apache.  this file would go in cgi-bin.
$SERVER_NAME=(Hidden for security purposes)
$SERVER_ADMIN=(Hidden for security purposes)
$SCRIPT_FILENAME=(Hidden for security purposes)
$SERVER_SOFTWARE=(Hidden for security purposes)
$SERVER_PORT=(Hidden for security purposes)
$SERVER_SIGNATURE=(Hidden for security purposes)
$SERVER_ADDR=(Hidden for security purposes)
$DOCUMENT_ROOT=(Hidden for security purposes)
$NOCOLOR_NEXT=1
# change the internal color format to html.
supermap html
# typical cw definition instructions.
other unset PWD;env
random brown:green:cyan
base random
match random+:random& (
match random+:default )
match white:default =
token random+:default 0 61
# web browser interprets this. (color conversions with the
# 'print' instruction need to happen after a 'supermap'
# and/or 'random' instruction, if they are present in the
# definition file)
print Content-type: text/html\n\n
print <head>\n
print <title>[cw-test.cgi]: color test.cgi</title>\n
print </head>\n
print <body bgcolor="#000000" text="#ffffff">\n
print <hr><h1>\n
print \C[random+]cw-test.cgi\C[random&]:\C[random] env display\n
print </h1><hr><br>\n
