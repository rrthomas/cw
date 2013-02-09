# makefiles may include programs colored by cw, and the coloring could
# mess things up, so turn it off.
$NOCOLOR=1
# don't want to color kernel dialog compiling.
ifnarg menuconfig
match punctuation \.
match punctuation -
match punctuation /
match punctuation \\
match bright :
match bright '
match bright `
match bright "
match highlight &
match neutral =
match neutral \[
match neutral \]
match highlight \*\*\*
match neutral \*
match neutral checking
match highlight warning: 
match error error: 
match error Error
match warning Interrupt
