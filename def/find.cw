# for -exec's, if another color wrapped program is called it will not
# be colored. (lets this definition file take over)
$NOCOLOR=1
match bright:default /
match bright:default \[
match bright:default \]
match bright:default (
match bright:default )
match punctuation:default :
match punctuation:default ;
match punctuation:default \.
