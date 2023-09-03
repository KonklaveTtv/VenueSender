#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include "include/fileutils.h"

#include <string>
#include <iostream>

class MessageHandler {

public:
	// Enumeration to represent different types of message that can be encountered
    enum class MessageType {

    	// White
    	SPLASH_TITLE_VERSION_MESSAGE,
    	COPYRIGHT_INFO_MESSAGE,
        INITIALIZATION_MESSAGE,
        
    	// Green
        SETTINGS_UPDATED_MESSAGE,

    	// Orange
    	ENTER_CHOICE_MESSAGE,
    	EXITING_VENUESENDER_MESSAGE,
    	RETURN_TO_MAIN_MENU_MESSAGE,
		USE_SSL_CONFIG_MESSAGE,
        SSL_CERTIFICATE_PATH_CONFIG_MESSAGE,
        VERIFY_PEER_CONFIG_MESSAGE,
        VERIFY_HOST_CONFIG_MESSAGE,
        VERBOSE_CONFIG_MESSAGE,
        SENDER_EMAIL_CONFIG_MESSAGE,
        SMTP_USERNAME_CONFIG_MESSAGE,
        SMTP_SERVER_CONFIG_MESSAGE,
        SMTP_PORT_CONFIG_MESSAGE,
        SMTP_PASSWORD_CONFIG_MESSAGE,

        // Red
    	EXIT_CONFIRMATION_MESSAGE

    };

	// Function to handle errors and display appropriate messages based on the type of message
	static void handleMessageAndReturn(MessageType message);

};

#endif // MESSAGEHANDLER_H
