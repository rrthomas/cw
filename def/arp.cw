match neutral:default [a-fA-F0-9]{1,2}:[a-fA-F0-9]{1,2}:[a-fA-F0-9]{1,2}:[a-fA-F0-9]{1,2}:[a-fA-F0-9]{1,2}:[a-fA-F0-9]{1,2}
ifos sunos
match bright:none Device
match bright:none -----
ifos-else
match bright:none Address
match bright:default -
ifos <any>
match bright:default \.
match bright:default \[
match bright:default \]
match neutral:default <
match neutral:default >
match highlight:default (
match highlight:default )
