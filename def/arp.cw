match neutral [a-fA-F0-9]{1,2}:[a-fA-F0-9]{1,2}:[a-fA-F0-9]{1,2}:[a-fA-F0-9]{1,2}:[a-fA-F0-9]{1,2}:[a-fA-F0-9]{1,2}
ifos sunos
match bright Device
match bright -----
ifos-else
match bright Address
match bright -
ifos <any>
match bright \.
match bright \[
match bright \]
match neutral <
match neutral >
match highlight \(
match highlight \)
