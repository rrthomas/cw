ifnarg --help
match bright /
match default M
match default K
match highlight [^	]+
ifarg-else
match highlight \[
match highlight \]
match highlight <
match highlight >
match highlight :
match bright ,
match bright =
match bright -
ifarg <any>
match default Try
