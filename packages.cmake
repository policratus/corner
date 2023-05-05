set(CPACK_PACKAGE_CONTACT "policratus@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION "Real-time 2D measurements of objects based on visual markers")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/policratus/corner")
set(CPACK_PACKAGE_CONTACT "policratus@gmail.com")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Policratus")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS YES)
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)

set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.md")

include(CPack)
