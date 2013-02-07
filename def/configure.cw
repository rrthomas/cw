# Configure scripts may include some third party programs that may be
# colored by cw, which could mess up the configure script.
$NOCOLOR=1
command ./configure {}
match highlight:default yes$
match error:default no$
match punctuation:default \.
match punctuation:default -
match punctuation:default /
match punctuation:default \\
match bright:default :
match bright:default '
match bright:default `
match bright:default "
match highlight:default &
match neutral:default =
match neutral:default \[
match neutral:default \]
match highlight:default \*\*\*
match neutral:default \*
match neutral:default checking
match warning:default updating
match bright:default creating
match bright:default loading
match punctuation:default (cached)
match highlight:default warning: 
match error:default error: 
match error:default Error
match warning:default Interrupt
