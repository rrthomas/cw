ifarg -m
match lowlight:default _
match lowlight:default -
match neutral:none Cache
match neutral:none Num
match neutral:none Total
match neutral:none Size
match neutral:none Pages
match punctuation:none  .*
ifarg-else
match punctuation:default [[:digit:]]
match neutral:default -
match neutral:default \[
match neutral:default \]
ifarg <any>
match neutral:bright (
match neutral:default )
