path /bin:/usr/bin:/sbin:/usr/sbin:<env>
base cyan+
match cyan+:cyan :
ifarg -h:--help
match cyan+:cyan -
match grey:cyan \[
match grey:cyan \]
