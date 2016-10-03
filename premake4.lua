-- Define the install prefix
prefix = nil
    -- Installation under Linux
    if (os.is('linux')) then
        prefix = '/usr/local'
        os.execute('sudo chown -R `whoami` ' .. prefix .. ' && sudo chmod -R 751 ' .. prefix)

    -- Installation under Mac OS X
    elseif (os.is('macosx')) then
        prefix = '/usr/local'

    -- Other platforms
    else
        print(string.char(27) .. '[31mThe installation is not supported on your platform' .. string.char(27) .. '[0m')
        os.exit()
    end

solution 'tarsier'
    configurations {'Release', 'Debug'}
    location 'build'

    newaction {
        trigger = "install",
        description = "Install the library",
        execute = function ()
            os.execute('rm -rf ' .. path.join(prefix, 'include/tarsier'))
            os.execute('mkdir ' .. path.join(prefix, 'include/tarsier'))
            os.execute('cp -r source/. ' ..  path.join(prefix, 'include/tarsier'))
            print(string.char(27) .. '[32mTarsier library installed.' .. string.char(27) .. '[0m')
            os.exit()
        end
    }

    newaction {
        trigger = 'uninstall',
        description = 'Remove all the files installed during build processes',
        execute = function ()
            os.execute('rm -rf ' .. path.join(prefix, 'include/tarsier'))
            print(string.char(27) .. '[32mTarsier library uninstalled.' .. string.char(27) .. '[0m')
            os.exit()
        end
    }

    project 'tarsierTest'
        -- General settings
        kind 'ConsoleApp'
        language 'C++'
        location 'build'
        files {'source/**.hpp', 'test/**.hpp', 'test/**.cpp'}

        -- Declare the configurations
        configuration 'Release'
            targetdir 'build/Release'
            defines {'NDEBUG'}
            flags {'OptimizeSpeed'}
        configuration 'Debug'
            targetdir 'build/Debug'
            defines {'DEBUG'}
            flags {'Symbols'}

        -- Linux specific settings
        configuration 'linux'
            buildoptions {'-std=c++11'}
            linkoptions {'-std=c++11'}
            postbuildcommands {
                'rm -rf ' .. path.join(prefix, 'include/tarsier'),
                'mkdir ' .. path.join(prefix, 'include/tarsier'),
                'cp -r ../source/. ' ..  path.join(prefix, 'include/tarsier'),
            }

        -- Mac OS X specific settings
        configuration 'macosx'
            buildoptions {'-std=c++11', '-stdlib=libc++'}
            linkoptions {'-std=c++11', '-stdlib=libc++'}
            postbuildcommands {
                'rm -rf ' .. path.join(prefix, 'include/tarsier'),
                'mkdir ' .. path.join(prefix, 'include/tarsier'),
                'cp -r ../source/. ' ..  path.join(prefix, 'include/tarsier'),
            }
