base punctuation
ifarg --help
match neutral:default \?
match neutral:default \+
match neutral:default  - 
ifarg -
match highlight:default  \+ 
match lowlight:default  - 
ifarg <any>
match neutral:default -
match neutral:default :
match neutral:default \.
match neutral:default =
match punctuation:default /
match punctuation:default (
match punctuation:default )
match punctuation:default <
match punctuation:default >
match punctuation:default \[
match punctuation:default \]
match neutral:default tty
match neutral:default pts
match punctuation:default LOGIN
match neutral:default root
match bright:default ... .. ..:..
