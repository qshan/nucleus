
# Script to install given APP_NAME as a SystemD Service

# Fix Relocatable paths -  https://github.com/axomem/nucleus/issues/45
# Edit or generate SystemD unit file and conf file with appname - https://github.com/axomem/nucleus/issues/68


if(NUCLEUS_INSTALL_INCLUDED)
    message(STATUS "INSTALL was already included for ${NUCLEUS_INSTALL_INCLUDED}")
    message(FATAL_ERROR "At present INSTALL can only be included once.")
endif()

set(NUCLEUS_INSTALL_INCLUDED ${APP_NAME})

message(STATUS "Including Install configuration - run `make install` to install program as daemon")
if(WIN32)
    # TODO - support windows install - need to update based on linux, or remove if same
    install(TARGETS ${APP_NAME} COMPONENT ${APP_NAME})
    install(FILES ${APP_DIR}/${APP_NAME}.conf DESTINATION etc COMPONENT ${APP_NAME})
else()
install(TARGETS ${APP_NAME} COMPONENT ${APP_NAME})
install(DIRECTORY ${APP_DIR}/share/ DESTINATION share/${APP_NAME} USE_SOURCE_PERMISSIONS COMPONENT ${APP_NAME} PATTERN "__pycache__" EXCLUDE)

install(FILES ${APP_DIR}/${APP_NAME}.conf DESTINATION etc COMPONENT ${APP_NAME})
install(FILES ${APP_DIR}/deploy/${APP_NAME}.service DESTINATION lib/systemd/system COMPONENT daemon)
#install(CODE "MESSAGE(\"\n*** Daemon installed: Run `systemctl daemon-reload` to reload services after install.\")"
#             COMPONENT daemon)
#install(CODE "MESSAGE(\"*** Use `systemctl start|stop|enable|status ${APP_NAME}` for service control\")"
#             COMPONENT daemon)

#install(CODE "MESSAGE(\"\n*** Edit /etc/${APP_NAME}/${APP_NAME}.conf and add relevant configuration settings.\")"
#        COMPONENT ${APP_NAME})
endif()