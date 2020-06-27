inspect = require "inspect"
-- utility

-- http://lua-users.org/wiki/SortedIteration
local function __genOrderedIndex(t)
  local orderedIndex = {}
  for key in pairs(t) do
    table.insert(orderedIndex, key)
  end
  table.sort(orderedIndex)
  return orderedIndex
end

local function orderedNext(t, state)
  -- Equivalent of the next function, but returns the keys in the alphabetic
  -- order. We use a temporary ordered key table that is stored in the
  -- table being iterated.

  local key = nil
  --print("orderedNext: state = "..tostring(state) )
  if state == nil then
    -- the first time, generate the index
    t.__orderedIndex = __genOrderedIndex(t)
    key = t.__orderedIndex[1]
  else
    -- fetch the next value
    for i = 1, table.getn(t.__orderedIndex) do
      if t.__orderedIndex[i] == state then
        key = t.__orderedIndex[i + 1]
      end
    end
  end

  if key then
    return key, t[key]
  end

  -- no more value to return, cleanup
  t.__orderedIndex = nil
  return
end

local function ordered_pairs(t)
  -- Equivalent of the pairs() function on tables. Allows to iterate
  -- in order
  return orderedNext, t, nil
end


-- C++ API namespace
local cpp = _cpp
_cpp = nil

-- Core
local core_ref = cpp.core.get_instance()

-- Class for accessing filter parameters
local params_proxy = {}
params_proxy.__metatable = true

function params_proxy.new(weak_ptr_wrapper)
  return setmetatable({
    weak_ptr_wrapper = weak_ptr_wrapper
  }, params_proxy)
end

function params_proxy:get_wrapper()
  local ret = self.weak_ptr_wrapper:lock_get()
  if not ret then
    error("Filter expired")
  end
  return ret
end

function params_proxy:__index(key)
  if params_proxy[key] then
    return params_proxy[key]
  end
  local filter = self:get_wrapper():get_filter()
  if filter:has_number_parameter(key) then
    return filter:get_number_parameter(key)
  elseif filter:has_string_parameter(key) then
    local str = filter:get_string_parameter(key)
    if str == "__true" then
      return true
    elseif str == "__false" then
      return false
    else
      return str
    end
  elseif filter:has_parameter(key) then
    return filter:get_parameter(key)
  elseif filter:has_view_parameter(key) then
    return filter:get_view_parameter(key)
  end
end

function params_proxy:__newindex(key, value)
  local wrapper = self:get_wrapper()
  local filter = wrapper:get_filter()
  local status
  if filter:has_number_parameter(key) then
    status = filter:set_number_parameter(key, value)
  elseif filter:has_string_parameter(key) then
    if value == true then
      status = filter:set_string_parameter(key, "__true")
    elseif value == false then
      status = filter:set_string_parameter(key, "__false")
    else
      status = filter:set_string_parameter(key, value)
    end
  elseif filter:has_parameter(key) then
    if type(value) == "number" then
      value = params.constant(value)
    end
    status = filter:set_parameter(key, value)
  elseif filter:has_view_parameter(key) then
    status = filter:set_view_parameter(key, value)
  else
    error("invalid parameter")
  end
  wrapper:notify_filter_internal_changes()
  if status.change_input then
    wrapper:request_changing_input()
  end
  if #status.error > 0 then
    error(status.error)
  end
end

-- Class for manipulating filter (filter wrapper)
local filter_proxy = {}
filter_proxy.__metatable = true

function filter_proxy.new(weak_ptr_wrapper)
  return setmetatable({
    weak_ptr_wrapper = weak_ptr_wrapper
  }, filter_proxy)
end

function filter_proxy:get_wrapper()
  local ret = self.weak_ptr_wrapper:lock_get()
  if not ret then
    error("filter expired")
  end
  return ret
end

function filter_proxy:interrupt()
  self:get_wrapper():interrupt()
end

function filter_proxy:request_rendering()
  self:get_wrapper():request_rendering(-1)
end

function filter_proxy:await_completion()
  self:get_wrapper():get_filter():wait_until_completion()
end

function filter_proxy:connect_previous(previous)
  local this_wrapper = self:get_wrapper()
  local prev_wrapper = previous.weak_ptr_wrapper:lock_get()
  if not prev_wrapper then
    error("previous filter expired")
  end
  local err = this_wrapper:can_connect_to(prev_wrapper)
  if not err then
    err = this_wrapper:connect_to(prev_wrapper)
    if not err then
      core_ref:update_filter_groups()
    else
      error(err)
    end
  else
    error(err)
  end
end

function filter_proxy:connect_next(next)
  local this_wrapper = self:get_wrapper()
  local next_wrapper = next.weak_ptr_wrapper:lock_get()
  if not next_wrapper then
    error("next filter expired")
  end
  local err = next_wrapper:can_connect_to(this_wrapper)
  if not err then
    err = next_wrapper:connect_to(this_wrapper)
    if not err then
      core_ref:update_filter_groups()
    else
      error(err)
    end
  else
    error(err)
  end
end

function filter_proxy:remove()
  core_ref:remove_filter(self.weak_ptr_wrapper)
end

function filter_proxy:__index(key)
  if filter_proxy[key] then
    return filter_proxy[key]
  end
  if key == "params" then
    return params_proxy.new(self.weak_ptr_wrapper)
  elseif key == "auto_request" then
    return self:get_wrapper():is_auto_request()
  elseif key == "input" then
    return self:get_wrapper():get_input()
  elseif key == "output" then
    return self:get_wrapper():get_output()
  elseif key == "expired" then
    return not self:get_wrapper().weak_ptr_wrapper:lock_get()
  elseif key == "type" then
    return self:get_wrapper():get_type_name()
  end
end

function filter_proxy:__newindex(key, value)
  if key == "auto_request" then
    self:get_wrapper().auto_request = value
  elseif key == "input" then
    if value == nil then
      self:get_wrapper():disconnect_input()
    else
      error("only nil assignment is supported")
    end
  elseif key == "output" then
    if value == nil then
      self:get_wrapper():disconnect_output()
    else
      error("only nil assignment is supported")
    end
  else
    error("invalid assignment")
  end
end

function filter_proxy:__tostring()
  return self:get_wrapper():get_filter():get_name()
end

local function core_add_filter(filter_ptr)
  local weak_ptr_wrapper = core_ref:add_filter(filter_ptr)
  return filter_proxy.new(weak_ptr_wrapper)
end

-- Core functions
local core_mt = {}

core = setmetatable({}, core_mt)

local function wrap_filter_list(t)
  local ret = {}
  for i, v in ipairs(t) do
    ret[i] = filter_proxy.new(v)
  end
  return ret
end

function core_mt:__index(key)
  if core_mt[key] then
    return core_mt[key]
  end
  if key == "num_threads" then
    return core_ref:get_num_threads()
  elseif key == "filters" then
    return wrap_filter_list(core_ref:get_filters())
  elseif key == "external_input_filters" then
    return wrap_filter_list(core_ref:get_external_input_filters())
  elseif key == "external_output_filters" then
    return wrap_filter_list(core_ref:get_external_output_filters())
  elseif key == "filter_group_roots" then
    return wrap_filter_list(core_ref:get_filter_group_roots())
  elseif key == "pipeline_roots" then
    return wrap_filter_list(core_ref:get_pipeline_roots())
  elseif key == "stray_filters" then
    return wrap_filter_list(core_ref:get_stray_filters())
  elseif key == "await_completion" then
    return function()
      core_ref:await_completion()
    end
  end
end

function core_mt:__newindex(key, value)
  if key == "num_threads" then
    core_ref:set_num_threads(value)
  end
end

-- Filter creation functions
filters = {}
filters.fractals = {}

function filters.fractals.basic()
  return core_add_filter(cpp.filters.fractals.basic())
end

filters.renderers = {}

function filters.renderers.basic()
  return core_add_filter(cpp.filters.renderers.basic())
end

filters.images = {}

function filters.images.merge()
  return core_add_filter(cpp.filters.images.merge())
end

filters.inputs = {}

function filters.inputs.view()
  return core_add_filter(cpp.filters.inputs.view())
end

filters.outputs = {}

function filters.outputs.image()
  return core_add_filter(cpp.filters.outputs.image())
end

-- Processing 4d positions, rotations and views
local pos4d_mt = {}

local pos4d = setmetatable({}, pos4d_mt)

pos4d_mt.__index = cpp.geom4d.pos

function pos4d_mt:__call(array)
  array = array or { 0, 0, 0, 0 }
  return cpp.geom4d.pos(array)
end

function pos4d_mt:__tostring()
  return "pos4d"
end

local rot4d_mt = {}

local rot4d = setmetatable({}, rot4d_mt)

rot4d_mt.__index = cpp.geom4d.rot

function rot4d_mt:__call(array)
  array = array or { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }
  return cpp.geom4d.rot(array)
end

function rot4d_mt:__tostring()
  return "rot4d"
end

local view4d_mt = {}

local view4d = setmetatable({}, view4d_mt)

view4d_mt.__index = cpp.geom4d.view

function view4d:translate(pos, use_rotation, use_scale)
  print("asdf")
  use_rotation = use_rotation or false
  use_scale = use_scale or false
  return self:translate(pos, use_rotation, use_scale)
end

function view4d_mt:__call(pos, rot, scale)
  pos = pos or geom4d.pos()
  rot = rot or geom4d.rot()
  scale = scale or 4
  return cpp.geom4d.view(pos, rot, scale)
end

function view4d_mt:__tostring()
  return "view4d"
end

geom4d = {
  pos = pos4d,
  rot = rot4d,
  view = view4d,
}

-- Load help file
help = require "help"

-- Generating the help page
local function help_function(obj)
  if not obj.label then
    return
  end
  local ret = "Help: " .. obj.label .. " (" .. obj._dir .. ")\n"
  if type(obj.read_only) == "boolean" then
    if obj.read_only then
      ret = ret .. "Category: Property (read-only)\n"
    else
      ret = ret .. "Category: Property (read/write)\n"
    end
    if obj.type then
      ret = ret .. "Type: " .. obj.type .. "\n"
    else
      error("Missing type of property " .. obj._dir)
    end
    if obj.default then
      ret = ret .. "Default: " .. obj.default .. "\n"
    end
  elseif obj.args then
    ret = ret .. "Category: Function\n"
    if #obj.args > 0 then
      ret = ret .. "Arguments: \n"
      for i, v in ipairs(obj.args) do
        if not v.name then
          error("missing name of " .. obj._dir .. ".args." .. k)
        end
        if not v.type then
          error("missing type of " .. obj._dir .. ".args." .. k)
        end
        if not v.desc then
          error("missing description of " .. obj._dir .. ".args." .. k)
        end
        if type(v.default) == "nil" then
          ret = ret .. " - " .. v.name .. ": " .. v.desc .. "\n    type: " .. v.type .. "\n"
        else
          ret = ret .. " - " .. v.name .. ": " .. v.desc .. "\n    type: " .. v.type .. ", default: " .. v.default .. "\n"
        end
      end
    else
      ret = ret .. "Arguments: None\n"
    end
    if obj.returns then
      ret = ret .. "Returns: " .. obj.returns .. "\n"
    else
      ret = ret .. "Returns: nil\n"
    end
  elseif obj.enum_values then
    ret = ret .. "Category: Enum\nValues: \n"
    for k, v in ordered_pairs(obj.enum_values) do
      if type(k) ~= "string" then
        error("Non-string key in " .. obj._dir .. ".enum_values")
      end
      if not v.desc then
        error("missing description of " .. obj._dir .. ".enum_values." .. k)
      end
      ret = ret .. "\"" .. k .. "\"" .. ": " .. v.desc .. "\n"
    end
  else
    if obj ~= help and obj ~= help.classes then
      if string.match(obj._dir, "help.classes") then
        ret = ret .. "Category: Class\n"
      else
        ret = ret .. "Category: Namespace\n"
      end
    end
  end
  if obj.desc then
    ret = ret .. obj.desc .. "\n"
  end
  if obj.parameters then
    ret = ret .. "Parameters: \n"
    for k, v in ordered_pairs(obj.parameters) do
      if type(k) ~= "string" then
        error("non-string key in " .. obj._dir .. ".parameters")
      end
      if not v.type then
        error("missing type of " .. obj._dir .. ".parameters." .. k)
      end
      if not v.desc then
        error("missing description of " .. obj._dir .. ".parameters." .. k)
      end
      if type(v.default) == "nil" then
        error("missing default of " .. obj._dir .. ".parameters." .. k)
      end
      ret = ret .. " - " .. k .. ": " .. v.desc .. "\n    type: " .. v.type .. ", default: " .. v.default .. "\n"
    end
  end
  local children = ""
  local first = true
  for k, v in ordered_pairs(obj) do
    if type(k) ~= "string" then
      error("Non-string key in " .. obj._dir)
    end
    if type(v) == "table" and k ~= "_parent" then
      if v.label then
        if not first then
          children = children .. ", "
        end
        children = children .. k
        first = false
      end
    end
  end
  if #children > 0 then
    ret = ret .. "Children: " .. children .. "\n"
  end
  return ret
end

-- Help objects metatable

local help_mt = {}

function help_mt:__tostring()
  lasthelp = self
  lastparent = self.parent
  return self._help
end

function help_mt:__index(key)
  if key == "parent" then
    return self._parent
  end
end

function help_mt:__newindex(key, value)
  error("Cannot insert here")
end

help_mt.__metatable = true

-- Transform the help table into an usable state
local function help_transform(table, base)
  table._dir = base
  table._help = help_function(table)
  for k, v in pairs(table) do
    if type(v) == "table" and k ~= "_parent" then
      v._parent = table
      if not getmetatable(v) and v.label then
        help_transform(v, base .. "." .. k)
      end
    end
  end
  setmetatable(table, help_mt)
end

help_transform(help, "help")

help_tree = function(obj)
  if obj == nil then
    return cpp.print_table(help)
  end
  if type(obj) ~= "table" then
    return
  end
  return cpp.print_table(obj)
end

better_print = function(obj)
  local function process(item, path)
    local mt = getmetatable(item)
    local mt_type = type(mt)
    if mt_type ~= "nil" and (mt_type ~= "table" or mt.__tostring) then
      return tostring(item)
    else
      return item
    end
  end
  if type(obj) == "table" then
    local mt = getmetatable(obj)
    local mt_type = type(mt)
    if mt_type ~= "nil" and (mt_type ~= "table" or mt.__tostring) then
      print(obj)
    else
      print(inspect(obj, { process = process }))
    end
  else
    print(obj)
  end
end
