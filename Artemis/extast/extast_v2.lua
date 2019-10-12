#!/usr/bin/env lua5.3
local inspect = require('inspect')

local function join_str(ls, sep)
  local s = ''
  for i,v in ipairs(ls) do
    if i == 1 then
      s = v
    else
      s = s .. sep .. v
    end
  end
  return s
end

local ast_sel_idx = 1
local function next_sel_node(ast)
  while true do
    local n = ast[ast_sel_idx]
    if n == nil then
      return nil, nil
    end
    ast_sel_idx = ast_sel_idx + 1
    for i,v in ipairs(n) do
      if v[1] == 'select' and v.ja ~= nil then
        return n, i
      end
    end
  end
end

local function parse_object_tag(tag)
  local t = tag[1]
  if type(t) ~= 'string' then
    error('tag key error')
  end
  local s = '['..t..' '
  for k,v in pairs(tag) do
    if k ~= 1 then
      if v:find(' ') ~= nil or v:find('"') ~= nil then
        error('not supported character: space or double-quote')
      end
      s = s..k..'="'..v..'" '
    end
  end
  return string.sub(s, 1, -2)..']'
end

local function parse_dialog_text_tag(tag)
  local t = tag[1]
  if t == 'ruby' then
    return '[ruby text="' .. tag.text .. '"]'
  elseif t == 'rt2' then
    return '\\n'
  elseif t == 'exfont' then
    if tag.size ~= nil then
    end
  elseif t:sub(1,1) == '/' then
    return '[' .. t .. ']'
  else
    print('unknown tag:', t)
    return '[unk:' .. t .. ']'
  end
end

local function get_dialog_text(jatext)
  local s = ''
  for i,v in ipairs(jatext) do
    if type(v) == 'string' then
      s = s .. v
    elseif type(v) == 'table' then
      s = s .. parse_object_tag(v)
    end
  end
  return s
end

local function insert_select_text(ls, sel)
  for i,v in ipairs(sel) do
    table.insert(ls, v)
  end
end

local function exttext(ast)
  local ls = {}
  for i,v in ipairs(ast.text) do
    local sel = v.select
    if sel ~= nil then
      if sel.ja == nil then
        print('error in select', inspect(v))
      end
      insert_select_text(ls, sel.ja)
      local snode, i0 = next_sel_node(ast)
      local mwnode = snode[#sel.ja+1+i0-1]
      if mwnode ~= nil and mwnode.mw ~= nil then
        table.insert(ls, mwnode.mw)
      end
      goto cont
    end

    local name = v.name
    if name ~= nil then
      if name.ja == nil and name.name == nil then
        print('error in name.ja:', inspect(v))
      end
      table.insert(ls, name.ja or name.name)
    end
    local ja = v.ja
    if ja == nil then
      print("no ja:", inspect(v))
      goto cont
    end
    if #ja ~= 1 then
      print('error in ja field:', inspect(v))
    end
    table.insert(ls, get_dialog_text(ja[1]))
    ::cont::
  end
  return ls
end

local function read_ast_file(fname)
 local ast_func, err = loadfile(fname, 't')
  if ast_func == nil then
    print("err when loading", fname, ":", err)
    return nil
  end
  ast = nil
  ast_func()
  if ast == nil then
    print(fname, "'s format may be wrong: there is not 'ast' variable")
    return nil
  end
  return ast
end

local function extast(ast_file_name, txt_file_name)
  local ast = read_ast_file(ast_file_name)
  if ast == nil then
    return
  end
  
  local ls = exttext(ast)
  if ls ~= nil then
    local fs = io.open(txt_file_name, 'wb')
    if fs == nil then
      print("can't open file:", txt_file_name)
      return
    end
    fs:write('\xef\xbb\xbf')
    fs:write(join_str(ls, "\r\n"))
    fs:close()
  end
end

if arg[1] == nil or arg[2] == nil then
  print('usage: extast.lua <astfile> <output_txt>')
  return
end
extast(arg[1], arg[2])
