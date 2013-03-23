if not arg["--help"] then
  match("highlight", "[%d%.,]")
  match("lowlight", "/")
  match("lowlight", "%b[]")
  match("lowlight", "%b()")
  match("warning", "%*")
  match("highlight", "tcp")
  match("warning", "udp")
  match("highlight", "open")
  match("warning", "filtered")
  match("error", "closed")
  match("lowlight", "Port")
end
