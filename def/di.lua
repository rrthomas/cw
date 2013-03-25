if not arg["--help"] then
  match("lowlight", "[KMGT] ")
  match("highlight", "Mount")
  match("highlight", "Filesystem")
  match("highlight", "Size")
  match("highlight", "Used")
  match("highlight", "Avail")
  match("highlight", "%%Used")
  match("highlight", "fs Type")
  match("lowlight", "[%d%%%.]")
end
