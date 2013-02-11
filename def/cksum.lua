match("bright", "/")
if arg["--help"] then
  match("bright", "-")
else
  match("bright", "[^ ]+ [^ ]+")
end
