enable_language(OBJCXX)
function(BuildMacOSExecutable)
    # Create executable and set its properties
    add_executable(${SIERRA_APPLICATION_NAME} MACOSX_BUNDLE ${SOURCE_FILES})
    set_target_properties(${SIERRA_APPLICATION_NAME} PROPERTIES
            BUNDLE TRUE
            MACOSX_BUNDLE_BUNDLE_NAME ${SIERRA_APPLICATION_NAME}
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.sierra.${SIERRA_APPLICATION_NAME}"
            MACOSX_BUNDLE_PRODUCT_IDENTIFIER "com.sierra.${SIERRA_APPLICATION_NAME}"
            MACOSX_BUNDLE_BUNDLE_VERSION "${SIERRA_APPLICATION_VERSION_MAJOR}.${SIERRA_APPLICATION_VERSION_MINOR}.${SIERRA_APPLICATION_VERSION_PATCH}"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "${SIERRA_APPLICATION_VERSION_MAJOR}"
    )

    # Link icon with the application
    set_source_files_properties(${SIERRA_APPLICATION_ICON_ICNS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    target_sources(${SIERRA_APPLICATION_NAME} PRIVATE ${SIERRA_APPLICATION_ICON_ICNS})

    if(SIERRA_PLATFORM_iOS)
        # Set Xcode project's info.plist
        set_target_properties(${SIERRA_APPLICATION_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${SIERRA_DIRECTORY}/src/Core/Platform/iOS/config/plist.in)
    endif()
endfunction()