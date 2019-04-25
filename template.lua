local json = (loadfile 'third_party/json.lua/json.lua')()

-- check throws an error if the given variable does not exist or does not have the expected type.
local function check(name, value, expected_type, json_type_name)
    if value == nil then
        error(name .. ' is not defined')
    end
    if type(value) ~= expected_type then
        error(name .. ' has an incorrect type (expected ' .. json_type_name .. ')')
    end
end

return function(configuration_filename)
    local configuration_file = io.open(configuration_filename, 'r')
    if configuration_file == nil then
        error('\'' .. configuration_filename .. '\' could not be open for reading')
    end
    configuration = json.decode(configuration_file:read('*a'))
    configuration_file:close()
    if type(configuration) ~= 'table' then
        error('\'' .. configuration_filename .. '\' does not contain an object')
    end
    check(
        '\'' .. 'configuration_filename' .. '\'',
        configuration,
        'table',
        'an object')
    check(
        '\'' .. configuration_filename .. '\'[\'filename\']',
        configuration['filename'],
        'string',
        'a string')
    check(
        '\'' .. configuration_filename .. '\'[\'name\']',
        configuration['name'],
        'string',
        'a string')
    check(
        '\'' .. configuration_filename .. '\'[\'description\']',
        configuration['description'],
        'table',
        'an array')
    check(
        '\'' .. configuration_filename .. '\'[\'template_parameters\']',
        configuration['template_parameters'],
        'table',
        'an array')
    check(
        '\'' .. configuration_filename .. '\'[\'parameters\']',
        configuration['parameters'],
        'table',
        'an array')
    check(
        '\'' .. configuration_filename .. '\'[\'input\']',
        configuration['input'],
        'table',
        'an object')
    for key, value in pairs(configuration['description']) do
        if type(key) ~= 'number' then
            error('\'' .. configuration_filename .. '\'[\'description\'] is not an array')
        end
        check(
            '\'' .. configuration_filename .. '\'[\'description\'][' .. tostring(key - 1) .. ']',
            value,
            'string',
            'a string')
    end
    for key, value in pairs(configuration['template_parameters']) do
        if type(key) ~= 'number' then
            error('\'' .. configuration_filename .. '\'[\'template_parameters\'] is not an array')
        end
        local name = '\'' .. configuration_filename .. '\'[\'template_parameters\'][' .. tostring(key - 1) .. ']'
        check(
            name,
            value,
            'table',
            'an object')
        check(
            name .. '[\'name\']',
            value['name'],
            'string',
            'a string')
        check(
            name .. '[\'type\']',
            value['type'],
            'string',
            'a string',
            true)
    end
    for key, value in pairs(configuration['parameters']) do
        if type(key) ~= 'number' then
            error('\'' .. configuration_filename .. '\'[\'parameters\'] is not an array')
        end
        local name = '\'' .. configuration_filename .. '\'[\'parameters\'][' .. tostring(key - 1) .. ']'
        check(
            name,
            value,
            'table',
            'an object')
        check(
            name .. '[\'name\']',
            value['name'],
            'string',
            'a string')
        check(
            name .. '[\'type\']',
            value['type'],
            'string',
            'a string')
        check(
            name .. '[\'store\']',
            value['store'],
            'string',
            'a string')
        if value['store'] ~= nil
            and value['store'] ~= 'no'
            and value['store'] ~= 'mutable'
            and value['store'] ~= 'constant'
            and value['store'] ~= 'forward' then
            error(name .. '[\'store\'] must be one of {\'mutable\', \'constant\', \'forward\'}')
        end
    end
    check(
        '\'' .. configuration_filename .. '\'[\'input\'][\'name\']',
        configuration['input']['name'],
        'string',
        'a string')
    check(
        '\'' .. configuration_filename .. '\'[\'input\'][\'type\']',
        configuration['input']['type'],
        'string',
        'a string')
    local output_file = io.open(configuration['filename'], 'w')
    if output_file == nil then
        error('\'' .. configuration['filename'] .. '\' could not be open for writing')
    end
    local template_parameters = {}
    local template_parameters_names = {}
    for index, parameter in ipairs(configuration['template_parameters']) do
        template_parameters[index] = parameter['type'] .. ' ' .. parameter['name']
        template_parameters_names[index] = parameter['name']
    end
    local parameters = {}
    local parameters_names = {}
    for index, parameter in ipairs(configuration['parameters']) do
        parameters[index] = parameter['type'] .. ' ' .. parameter['name']
        if parameter['store'] == 'forward' then
            parameters_names[index] = 'std::forward<' .. parameter['type'] .. '>(' .. parameter['name'] .. ')'
        else
            parameters_names[index] = parameter['name']
        end
    end
    local initialization_parameters = {}
    local has_initialization_parameters = false
    for index, parameter in ipairs(configuration['parameters']) do
        if parameter['store'] ~= 'no' then
            has_initialization_parameters = true
            if parameter['store'] == 'forward' then
                initialization_parameters[index] = '_' ..
                    parameter['name'] ..
                    '(std::forward<' ..
                    parameter['type'] ..
                    '>(' ..
                    parameter['name'] ..
                    '))'
            else
                initialization_parameters[index] = '_' .. parameter['name'] .. '(' .. parameter['name'] .. ')'
            end
        end
    end
    output_file:write(
        '#pragma once\n\n',
        '#include <utility>\n\n',
        '/// tarsier is a collection of event handlers.\n',
        'namespace tarsier {\n')
    for key, value in pairs(configuration['description']) do
        output_file:write('    /// ', value, '\n')
    end
    output_file:write(
        '    template <', table.concat(template_parameters, ', '), '>\n',
        '    class ', configuration['name'], ' {\n',
        '        public:\n',
        '        ', configuration['name'], '(', table.concat(parameters, ', '), ')')
    if has_initialization_parameters then
        output_file:write(' :\n',
            '            ', table.concat(initialization_parameters, ', '))
    end
    output_file:write(' {\n',
        '        }\n',
        '        ', configuration['name'], '(const ', configuration['name'], '&) = delete;\n',
        '        ', configuration['name'], '(', configuration['name'], '&&) = default;\n',
        '        ', configuration['name'], '& operator=(const ', configuration['name'], '&) = delete;\n',
        '        ', configuration['name'], '& operator=(', configuration['name'], '&&) = default;\n',
        '        virtual ~', configuration['name'], '() {}\n\n',
        '        /// operator() handles an event.\n',
        '        virtual void operator()(', configuration['input']['type'], ' ', configuration['input']['name'], ') {\n',
        '        }\n\n',
        '        protected:\n')
    for index, parameter in ipairs(configuration['parameters']) do
        output_file:write('        ')
        if parameter['store'] ~= 'no' then
            if parameter['store'] == 'constant' then
                output_file:write('const ')
            end
            output_file:write(parameter['type'], ' _', parameter['name'], ';\n')
        end
    end
    output_file:write('    };\n\n',
        '    /// make_', configuration['name'], ' creates a')
    if string.find(configuration['name'], '^\s*[AEIOUaeiou]') then
        output_file:write('n')
    end
    output_file:write(' ', configuration['name'], ' from functors.\n',
        '    template <', table.concat(template_parameters, ', '), '>\n',
        '    inline ', configuration['name'], '<', table.concat(template_parameters_names, ', '), '> make_', configuration['name'], '(\n',
        '        ',  table.concat(parameters, ', '), ') {\n',
        '        return ', configuration['name'], '<', table.concat(template_parameters_names, ', '), '>(\n',
        '            ', table.concat(parameters_names, ', '), ');\n',
        '    }\n',
        '}\n')
    output_file:close()
end
