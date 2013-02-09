match bright [[:digit:]]
match warning /
match bright -
match bright \+
match highlight \[
match highlight \]
match warning \([^)]*\)
match neutral \*
match punctuation UDP
match punctuation TCP
match punctuation TPI
ifos linux
match punctuation REG
match punctuation DIR
match punctuation CHR
match punctuation FIFO
match error DEL
ifos-else
match punctuation VREG
match punctuation VDIR
match punctuation VCHR
match punctuation VFIFO
match error VDEL
ifos <any>
match default COMMAND
ifnarg -h:--help
match highlight [^ ]+
