
# Script to install given APP_NAME as a SystemD Service

# Fix Relocatable paths -  https://github.com/axomem/nucleus/issues/45

if(NUCLEUS_INSTALL_INCLUDED)
    message(STATUS "INSTALL was already included for ${NUCLEUS_INSTALL_INCLUDED}")
    message(FATAL_ERROR "At present INSTALL can only be included once.")
endif()

set(NUCLEUS_INSTALL_INCLUDED ${APP_NAME})

message(STATUS "Including Install configuration - run `make install` to install program as daemon")

install(TARGETS ${APP_NAME} DESTINATION /usr/local/bin COMPONENT ${APP_NAME})
install(FILES nucleus.conf DESTINATION /etc/${APP_NAME} RENAME ${APP_NAME}.conf COMPONENT ${APP_NAME})
install(FILES util/nucleus.service DESTINATION /usr/lib/systemd/system RENAME ${APP_NAME}.service COMPONENT daemon)
install(CODE "MESSAGE(\"\n*** Edit /etc/${APP_NAME}/${APP_NAME}.conf and add relevant configuration settings.\")"
             COMPONENT ${APP_NAME})
install(CODE "MESSAGE(\"\n*** Daemon installed: Run `systemctl daemon-reload` to reload services after install.\")"
             COMPONENT daemon)
install(CODE "MESSAGE(\"*** Use `systemctl start|stop|enable|status ${APP_NAME}` for service control\")"
             COMPONENT daemon)
