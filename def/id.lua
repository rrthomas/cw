-- shell scripts use this a lot, will cause problems being colored.
if not arg["-g"] and not arg["-G"] and not arg["-n"] and not arg["-r"] and not arg["-u"] and not arg["--group"] and not arg["--groups"] and not arg["--name"] and not arg["--real"] and not arg["--user"] then
  match("bright", ":")
  match("bright", "=")
  match("bright", "/")
  match("bright", "%b[]")
  match("bright", "%b()")
end
if arg["--help"] then
  match("bright", "%-")
end
