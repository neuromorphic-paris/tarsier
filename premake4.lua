local template = require 'template'

newaction {
    trigger = 'template',
    description = 'generate a template event handler',
    execute = function()
        if _OPTIONS['configuration'] == nil then
            template('configuration.json')
        else
            template(_OPTIONS['configuration'])
        end
    end
}
newoption {
    trigger = 'configuration',
    value = '/path/to/configuration.json',
    description = 'set the path to the configuration file for template generation'
}

solution 'tarsier'
    configurations {'release', 'debug'}
    location 'build'
    project 'tarsier'
        kind 'ConsoleApp'
        language 'C++'
        location 'build'
        files {'source/*.hpp', 'test/*.cpp'}
        configuration 'release'
            targetdir 'build/release'
            defines {'NDEBUG'}
            flags {'OptimizeSpeed'}
        configuration 'debug'
            targetdir 'build/debug'
            defines {'DEBUG'}
            flags {'Symbols'}
        configuration 'linux'
            links {'pthread'}
            buildoptions {'-std=c++11'}
            linkoptions {'-std=c++11'}
        configuration 'macosx'
            buildoptions {'-std=c++11'}
            linkoptions {'-std=c++11'}
        configuration 'windows'
            files {'.clang-format'}
