path /sbin:/usr/sbin:/bin:/usr/bin:<env>
base cyan
regex white:default [a-fA-F0-9]{1,2}:[a-fA-F0-9]{1,2}:[a-fA-F0-9]{1,2}:[a-fA-F0-9]{1,2}:[a-fA-F0-9]{1,2}:[a-fA-F0-9]{1,2}
ifos sunos
match cyan+:none Device
match cyan+:none -----
ifos-else
match cyan+:none Address
match cyan+:default -
ifos <any>
match cyan+:default .
match cyan+:default [
match cyan+:default ]
match white:default <
match white:default >
match green+:default (
match green+:default )
