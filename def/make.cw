# makefiles may include programs colored by cw, and the coloring could
# mess things up, so turn it off.
$NOCOLOR=1
# don't want to color kernel dialog compiling.
ifnarg menuconfig
match punctuation:default \.
match punctuation:default -
match punctuation:default /
match punctuation:default \
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
match highlight:default warning: 
match error:default error: 
match error:default Error
match warning:default Interrupt
