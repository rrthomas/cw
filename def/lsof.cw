match bright:punctuation [[:digit:]]
match warning:default /
match bright:default -
match bright:default \+
match highlight:default \[
match highlight:default \]
match highlight:warning (
match highlight:default )
match neutral:default \*
match punctuation:default UDP
match punctuation:default TCP
match punctuation:default TPI
ifos linux
match punctuation:default REG
match punctuation:default DIR
match punctuation:default CHR
match punctuation:default FIFO
match error:default DEL
ifos-else
match punctuation:default VREG
match punctuation:default VDIR
match punctuation:default VCHR
match punctuation:default VFIFO
match error:default VDEL
ifos <any>
match default:default COMMAND
ifnarg -h:--help
match highlight:default [^ ]+
