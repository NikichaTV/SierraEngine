# Disable automatic reference counting
set_xcode_property(Sierra CLANG_ENABLE_OBJC_ARC "NO" "All")
set_xcode_property(Sierra GCC_WARN_INHIBIT_ALL_WARNINGS "YES" "All")

# Link source files
target_sources(Sierra PRIVATE
    iOSContext.mm
    iOSContext.h
    UIKitContext.mm
    UIKitContext.h
    UIKitScreen.mm
    UIKitScreen.h
    UIKitSelectorBridge.mm
    UIKitSelectorBridge.h
    UIKitTemporaryCreateInfoStorage.mm
    UIKitTemporaryCreateInfoStorage.h
    UIKitTouchManager.mm
    UIKitTouchManager.h
    UIKitWindow.mm
    UIKitWindow.h
)

# Link libraries
target_link_libraries(Sierra PRIVATE
    "-framework UIKit"
    "-framework QuartzCore"
)