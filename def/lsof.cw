base cyan
match cyan+:cyan [[:digit:]]
match yellow:default /
match cyan+:default -
match cyan+:default \+
match green+:default \[
match green+:default \]
match green+:yellow (
match green+:default )
match white:default \*
match blue+:default UDP
match blue+:default TCP
match blue+:default TPI
ifos linux
match grey:default REG
match grey+:default DIR
match purple:default CHR
match purple+:default FIFO
match red:default DEL
ifos-else
match grey:default VREG
match grey+:default VDIR
match purple:default VCHR
match purple+:default VFIFO
match red:default VDEL
ifos <any>
match default:default COMMAND
ifnarg -h:--help
match green+:default [^ ]+
