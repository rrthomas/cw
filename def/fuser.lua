if not arg["-i"] and not arg["--interactive"] and not arg["-k"] and not arg["--kill"] then
  match("lowlight", "[%-/%.]")
  match("highlight", "[:|]")
  match("lowlight", "%b[]")
  match("lowlight", "%b()")
  if #arg > 0 then
    match("lowlight", "%d")
    match("lowlight", "USER")
  end
end
