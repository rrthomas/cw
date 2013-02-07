match bright:default [[:digit:]]
match punctuation:default \[
match punctuation:default \]
match neutral:default (
match neutral:default )
match neutral:default \+
match bright:default -
match highlight:default :
match error:default root
ifnarg --help
match bright:default [^ ]+
