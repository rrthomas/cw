-- Rockspec data

-- Variables to be interpolated:
--
-- package
-- version

local default = {
  package = package_name,
  version = version.."-1",
  source = {
    url = "git://github.com/rrthomas/"..package_name..".git",
  },
  description = {
    summary = "Colorer for command-line programs",
    detailed = [[
      cw is a colorer for command-line programs.  It is designed to simulate
      the environment of the commands being executed, so that when you type
      'du', 'df', 'ping', etc. in your shell the output is automatically
      colored according to a definition script.  You can easily change
      or add new scripts, and change the colors used.
     ]],
    homepage = "http://github.com/rrthomas/"..package_name.."/",
    license = "GPL",
  },
  dependencies = {
    "lua >= 5.1",
    "stdlib >= 34",
    "luaposix >= 5.1.26",
    "ansicolors >= 1.0.2",
  },
  build = {
    type = "command",
    build_command = "LUA=$(LUA) ./configure --prefix=$(PREFIX) --libdir=$(LIBDIR) --datadir=$(LUADIR) && make clean && make",
    install_command = "make install",
    copy_directories = {},
  },
}

if version ~= "git" then
  default.source.branch = "release-v"..version
else
  default.build.build_command = "./bootstrap && " .. default.build.build_command
  table.insert (default.dependencies, "ldoc")
end

return {default=default, [""]={}}
