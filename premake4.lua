solution 'tarsier'
    configurations {'release', 'debug'}
    location 'build'
    project 'tarsier'
        kind 'ConsoleApp'
        language 'C++'
        location 'build'
        files {'source/*.hpp', 'test/*.cpp'}
        buildoptions {'-std=c++11'}
        linkoptions {'-std=c++11'}
        configuration 'release'
            targetdir 'build/release'
            defines {'NDEBUG'}
            flags {'OptimizeSpeed'}
        configuration 'debug'
            targetdir 'build/debug'
            defines {'DEBUG'}
            flags {'Symbols'}
