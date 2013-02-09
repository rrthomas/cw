ifarg -m
match lowlight _
match lowlight -
match neutral Cache
match neutral Num
match neutral Total
match neutral Size
match neutral Pages
match punctuation  .*
ifarg-else
match punctuation [[:digit:]]
match neutral -
match neutral \[
match neutral \]
ifarg <any>
match bright \([^)]*\)
