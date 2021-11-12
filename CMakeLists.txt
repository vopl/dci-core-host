include(dciIntegrationUnit)
dciIntegrationUnit(host
        INCLUDE_DIRS include
        CMM_DIRS cmakeModules
        IDL_DIRS idl
        DEPENDS poll aup config stiac)

############################################################
file(GLOB_RECURSE INC include/*)
file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE IDL idl/*)
file(GLOB_RECURSE CMM cmakeModules/*)

############################################################
add_library(${UNAME} SHARED ${INC} ${SRC} ${IDL} ${CMM})
dciIntegrationSetupTarget(${UNAME})
#target_include_directories(${UNAME} PRIVATE src)

target_link_libraries(${UNAME} PRIVATE
#    himpl
    mm
    cmt
    idl
    bytes
    poll
    sbs
    test
    utils
    config
    stiac
    exception
)

target_link_libraries(${UNAME} PRIVATE
    dl
    boost_program_options)

############################################################
include(dciHimpl)
dciHimplMakeLayouts(${UNAME}
    INCLUDE
        src

    OUTPUT
        dci/host/implMetaInfo.hpp

    HEADERS
        impl/manager.hpp

    CLASSES
        dci::host::impl::Manager
)

##############################################################
include(dciIdl)
dciIdl(${UNAME} cpp
    INCLUDE ${DCI_IDL_DIRS}
    SOURCES host.idl
    OPTIONS --cpp-stiac-support --cpp-stiac-support-filter "^dci::idl::gen::host::"
    NAME idl-host
)

################################################################
add_executable(${UNAME}-executable src-executable/main.cpp)
dciIntegrationSetupTarget(${UNAME}-executable)
set_target_properties(${UNAME}-executable PROPERTIES OUTPUT_NAME ${UNAME})
target_link_libraries(${UNAME}-executable PRIVATE
    ${UNAME}
    mm
    cmt
    sbs
    exception
    aup
    poll
    integration
    boost_program_options
)

############################################################
add_test(NAME noenv COMMAND ${UNAME}-executable --test noenv)
add_test(NAME mnone COMMAND ${UNAME}-executable --test mnone)
add_test(NAME mstart COMMAND ${UNAME}-executable --test mstart)
