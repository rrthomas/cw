posix.setenv("NOCOLOR", "1")
command = "ltrace " .. table.concat(arg, " ") .. " 2>&1"
match("bright", "%+%+%+")
match("bright", "---")
