command = "tail -f /var/log/syslog"
match("bright", "%b()")
match("bright", "%b[]")
match("highlight", "^%a+ +%d+ +[%d:]+ [^ ]+")
match("neutral", "^%a+ +%d+ +[%d:]+")
