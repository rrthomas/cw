# shell scripts use this a lot, will cause problems being colored.
ifnarg -g:-G:-n:-r:-u:--group:--groups:--name:--real:--user
match bright :
match bright =
match bright /
match bright \[[^]]*\]
match bright \([^)]*\)
ifarg --help
match bright -
