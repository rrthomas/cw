-- Do not color the program being traced.
posix.setenv("NOCOLOR", "1")
command = "strace " .. table.concat(arg, " ") .. " 2>&1"
match("bright", "%(.+%).*[ ]=[ ]")
match("bright", "%+%+%+")
match("bright", "---")
