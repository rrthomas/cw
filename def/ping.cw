ifos sunos
match bright:punctuation ----
match neutral:default is alive
ifos <any>
match bright:punctuation ---
ifnarg --help
match highlight:bright =
ifarg <any>
match neutral:bright :
match neutral:highlight (
match neutral:default )
match highlight:bright \[
match highlight:default \]
match bright:default ,
match highlight:default  ms
match none:highlight  from 
match none:highlight From 
match punctuation:none PING
