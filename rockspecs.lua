-- Rockspec data

-- Variables to be interpolated:
--
-- package_name
-- version

local default = {
  package = package_name,
  version = version.."-1",
  source = {
    url = "git://github.com/rrthomas/"..package_name..".git",
  },
  description = {
    summary = "Colorer for command-line programs",
    detailed = [[cw is a colorer for command-line programs.  It is designed to simulate
      the environment of the commands being executed, so that when you type
      'du', 'df', 'ping', etc. in your shell the output is automatically
      colored according to a definition script.  You can easily change
      or add new scripts, and change the colors used.
     ]],
    homepage = "http://github.com/rrthomas/"..package_name.."/",
    license = "GPL",
  },
  dependencies = {
    "lua >= 5.2",
    "stdlib >= 41.2",
    "optparse >= 1.4",
    "luaposix >= 33.3.1",
    "ansicolors >= 1.0.2",
    "ldoc",
  },
  build = {
    type = "make",
    variables = { LUA = "$(LUA)", prefix = "$(PREFIX)" },
    copy_directories = {},
  },
}

if version ~= "git" then
  default.source.branch = "v"..version
end

return {default=default, [""]={}}
