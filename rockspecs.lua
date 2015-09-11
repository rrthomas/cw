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
    "lua >= 5.2, < 5.4",
    "stdlib ~> 41.2",
    "luaposix >= 33.3.1, < 33.4",
    "ansicolors >= 1.0.2",
    "ldoc",
  },
  build = {
    type = "command",
    build_command = "autoreconf -i && LUA=$(LUA) ./configure --prefix=$(PREFIX) --libdir=$(LIBDIR) --datadir=$(LUADIR) && make LUA=$(LUA) prefix=$(PREFIX) clean all",
    install_command = "make LUA=$(LUA) prefix=$(PREFIX) install",
    copy_directories = {},
  },
}

if version ~= "git" then
  default.source.branch = "v"..version
end

return {default=default, [""]={}}
