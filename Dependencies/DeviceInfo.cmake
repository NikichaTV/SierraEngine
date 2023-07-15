# ========== SYSTEM & ARCHITECTURE SETTINGS ========== #
set(DEVICE_OS "Undefined")
set(DEVICE_ARCHITECTURE "Undefined")

if(WIN32)
    set(DEVICE_OS "Windows")
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        message("System is Windows x32")
        set(DEVICE_ARCHITECTURE "x32")

    else()
        message("System is Windows x64")
        set(DEVICE_ARCHITECTURE "x64")

    endif()

elseif(APPLE)
    set(DEVICE_OS "macOS")
    if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64" OR "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "AMD64")
        message("System is macOS x86-64 or amd64")
        set(DEVICE_ARCHITECTURE "x64")

    elseif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm64")
        message("System is macOS arm64")
        set(DEVICE_ARCHITECTURE "arm64")

    elseif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "arm32")
        message("System is macOS arm32")
        set(DEVICE_ARCHITECTURE "arm32")

    else()
        message("System is macOS")
    endif()

elseif(UNIX)
    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
        set(DEVICE_OS "Linux")
        if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64" OR "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "AMD64")
            message("System is Linux x86-64 or amd64")
            set(DEVICE_ARCHITECTURE "x64")

        elseif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "aarch64")
            message("System is Linux arm64")
            set(DEVICE_ARCHITECTURE "arm64")

        elseif("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "aarch32")
            message("System is Linux arm32")
            set(DEVICE_ARCHITECTURE "arm32")

        else()
            message("System is Linux")
        endif()

    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
        set(DEVICE_OS "Android")
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            message("Android architecture is 64-bit")
            set(DEVICE_ARCHITECTURE "x64")

        elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
            message("System is Android x32")
            set(DEVICE_ARCHITECTURE "x32")

        else()
            message("System is Android")
        endif()

    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS")
        set(DEVICE_OS "iOS")
        if("${CMAKE_OSX_ARCHITECTURES}" MATCHES "arm64")
            message("System is iOS ARM64")
            set(DEVICE_ARCHITECTURE "x64")

        elseif("${CMAKE_OSX_ARCHITECTURES}" MATCHES "x86_64")
            message("System is iOS x86_64")
            set(DEVICE_ARCHITECTURE "x64")

        else()
            message("System is iOS")
        endif ()
    else()
        message("System is UNIX-like")
        # Perform actions specific to other UNIX-like systems
    endif()

else()
    message("Device system is not recognized")
endif()