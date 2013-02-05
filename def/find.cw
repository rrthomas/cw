# for -exec's, if another color wrapped program is called it will not
# be colored. (lets this definition file take over)
$NOCOLOR=1
base cyan
match cyan+:default /
match cyan+:default \[
match cyan+:default \]
match cyan+:default (
match cyan+:default )
match grey:default :
match grey:default ;
match grey:default \.
