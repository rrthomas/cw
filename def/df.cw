ifarg --help
base punctuation
match neutral:default -
ifarg-else
match default:punctuation %
match default:punctuation K 
match default:punctuation M 
match default:punctuation G 
match default:punctuation T 
match default:none K-blocks
match default:none k-blocks
match default:none -blocks
match bright:default Mounted on
ifarg <any>
match neutral:punctuation [[:digit:]]
match punctuation:punctuation /
match punctuation:punctuation \.
match punctuation:punctuation :
match punctuation:punctuation <
match punctuation:punctuation >
match punctuation:punctuation (
match punctuation:punctuation )
match punctuation:punctuation \[
match punctuation:punctuation \]
match neutral:punctuation `
match neutral:default '
