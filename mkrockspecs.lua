-- Generate rockspecs from a prototype with variants

local tree = require "std.tree"

if select ("#", ...) < 2 then
  io.stderr:write "Usage: mkrockspecs PACKAGE VERSION\n"
  os.exit ()
end

package_name = select (1, ...)
version = select (2, ...)

function format (x, indent)
  indent = indent or ""
  if type (x) == "table" then
    local s = "{\n"
    for i, v in pairs (x) do
      if type (i) ~= "number" then
        local key = i
        if type (key) == "string" and not key:match("^[%w_]+$") then
          key = "["..format (key).."]"
        end
        s = s..indent..key.." = "..format (v, indent.."  ")..",\n"
      end
    end
    for i, v in ipairs (x) do
      s = s..indent..format (v, indent.."  ")..",\n"
    end
    return s..indent:sub (1, -3).."}"
  elseif type (x) == "string" then
    return string.format ("%q", x)
  else
    return tostring (x)
  end
end

flavour = "" -- a global, visible in loadfile
for f, spec in pairs (loadfile ("rockspecs.lua") ()) do
  if f ~= "default" then
    local specfile = package_name.."-"..(f ~= "" and f:lower ().."-" or "")..version.."-1.rockspec"
    h = io.open (specfile, "w")
    assert (h)
    flavour = f
    local specs = loadfile ("rockspecs.lua") () -- reload to get current flavour interpolated
    local spec = tree.merge (tree (specs.default), tree (specs[f]))
    local s = ""
    for i, v in pairs (spec) do
      s = s..i.." = "..format (v, "  ").."\n"
    end
    h:write (s)
    h:close ()
    os.execute ("luarocks lint " .. specfile)
  end
end
