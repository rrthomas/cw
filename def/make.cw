# makefiles may include programs colored by cw, and the coloring could
# mess things up, so turn it off.
$NOCOLOR=1
# don't want to color kernel dialog compiling.
ifnarg menuconfig
base grey+
match cyan:default \.
match grey:default -
match grey:default /
match cyan:default \
match cyan+:default :
match cyan+:default '
match cyan+:default `
match cyan+:default "
match green+:default &
match white:default =
match white:default \[
match white:default \]
match green+:default \*\*\*
match white:default \*
match white:default checking
match green+:default warning: 
match red+:default error: 
match red+:default Error
match yellow:default Interrupt
