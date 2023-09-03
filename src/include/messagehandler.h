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
    	
    		// Splashscreen
	    	SPLASH_TITLE_VERSION_MESSAGE,
	    	COPYRIGHT_INFO_MESSAGE,
	        INITIALIZATION_MESSAGE,
        
    	// Green
    
    		// Email Config
	        SETTINGS_UPDATED_MESSAGE,

	        // Email
	        EMAIL_CREATION_SUCCESS_MESSAGE,

	        // Template
		    CONFIRM_SEND_TEMPLATE_MESSAGE,


    	// Orange

	    	// Menu
	    	ENTER_CHOICE_MESSAGE,
	    	RETURN_TO_MAIN_MENU_MESSAGE,
	    	EXITING_VENUESENDER_MESSAGE,

	    	// Email Config
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

	        // Email
	        ENTER_SUBJECT_FOR_EMAIL_MESSAGE,
	        ENTER_MESSAGE_FOR_EMAIL_MESSAGE,
	        CONFIRM_ADD_ATTACHMENT_MESSAGE,
	        ADD_ATTACHMENT_MESSAGE,
	        ADD_DIFFERENT_ATTACHMENT_MESSAGE,

        // Red

	        // Menu
	    	PRESS_RETURN_MESSAGE,
	    	EXIT_CONFIRMATION_MESSAGE,

	        // Email
		    ATTACHMENT_INVALID_CHOICE_ERROR,
		    MODIFY_EMAIL_CONFIRMATION_MESSAGE,

		    // Template
		    MODIFY_TEMPLATE_CONFIRMATION_MESSAGE
    };

	// Function to handle errors and display appropriate messages based on the type of message
	static void handleMessageAndReturn(MessageType message);

};

#endif // MESSAGEHANDLER_H
