-- Calling script must define dcs_install_path and module_name as global variables.
-- Examples below:
        -- dcs_install_path = [[C:\Program Files\Eagle Dynamics\DCS World OpenBeta]]
        -- module_name = "A-10C"

-- Define global class type for clickabledata.
class_type = 
{
	NULL   = 0,
	BTN    = 1,
	TUMB   = 2,
	SNGBTN = 3,
	LEV    = 4
}

-- Mock out the get_option_value and get_aircraft_type functions that don't exist in this environment.
function get_aircraft_type()
	return module_name
end
function get_option_value(x)
	return nil
end

-- Protect against relative path "dofile" calls by inspecting paths before running.
-- All should use the configured "LockOn_Options.script_path" variable.
call_dofile = dofile
function inspect_and_dofile(path)
	if (string.match(path, "DCS World")) then
		call_dofile(path)
	else

		call_dofile(dcs_install_path .. path)
	end
end
--Overwrite function so any dofile function calls by module scripts will go through inspect first.
dofile = inspect_and_dofile


function len(table)
	local count = 0
    for _ in pairs(table) do
	    count = count + 1
	end
	return count
end

function get_device_name(device_id)
	for device,id in pairs(devices) do
		if (id == device_id) then
			if (device == nil) then
				return ""
			else
				return device
			end
		end
	end
end

function get_class_enum_label(class)
	if class == 0 then
		return "NULL"
	elseif class == 1 then
		return "BTN"
	elseif class == 2 then
		return "TUMB"
	elseif class == 3 then
		return "SNGBTN"
	elseif class == 4 then
		return "LEV"
	else
		return ""
	end
end

function get_index_value(table, index)
	if (table ~= nil) then
		return table[index]
	end
	return ""
end

function collect_element_attributes(elements)
	collected_element_attributes = {}
	local count = 0
	for element_id,_ in pairs(elements) do
		local element_name = element_id
		local hint = elements[element_id].hint
		local classes = elements[element_id].class
		local args = elements[element_id].arg
		local arg_values = elements[element_id].arg_value
		local arg_lims = elements[element_id].arg_lim
		local device_id = elements[element_id].device
		local device_name = get_device_name(device_id)
		local command_ids = elements[element_id].action
		
		-- Iterate through classes
		for idx,class in pairs(classes) do
			local class_name = get_class_enum_label(class)
			local command_id = get_index_value(command_ids,idx)
			local arg = get_index_value(args,idx)
			local arg_value = get_index_value(arg_values,idx)
			local arg_lim = get_index_value(arg_lims,idx)
			local arg_lim1 = ""
			local arg_lim2 = ""
			if (arg_lim ~= nil) then
				if (type(arg_lim) == "table") then
					arg_lim1 = arg_lim[1]
					arg_lim2 = arg_lim[2]
				elseif (type(arg_lim) == "number") then
					arg_lim1 = arg_lim
				end
			end
			count = count + 1
			collected_element_attributes[count] = string.format('%s(%s),%s,%s,%s,%s,%s,%s,%s,%s',
					device_name, device_id, command_id, element_name, class_name, arg, arg_value, arg_lim1, arg_lim2, hint)
		end
	end
	return collected_element_attributes
end

function load_module(module_name)
	LockOn_Options = {}

	-- Specialty case handling for odd multi-version modules.
	if string.match(module_name,"C-101") then
		LockOn_Options.script_path = dcs_install_path..[[\Mods\aircraft\C-101\Cockpit\]]..module_name..[[\]]
	elseif string.match(module_name,"L-39") then
		L_39ZA = string.match(module_name, "L-39ZA")
		LockOn_Options.script_path = dcs_install_path..[[\Mods\aircraft\L-39C\Cockpit\]]
		dofile(LockOn_Options.script_path.."devices.lua")
	else
		LockOn_Options.script_path = dcs_install_path..[[\Mods\aircraft\]]..module_name..[[\Cockpit\]]
	end

	file_path_valid = loadfile(LockOn_Options.script_path.."clickabledata.lua")
	if file_path_valid == nil then
		LockOn_Options.script_path = LockOn_Options.script_path..[[Scripts\]]
	end

	dofile(LockOn_Options.script_path.."clickabledata.lua")
	
	element_list = collect_element_attributes(elements)
	return element_list
end

list = load_module(module_name)
return table.unpack(list)
