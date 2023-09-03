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
	        INIT_ENTER_PASSWORD_MESSAGE,
	        INIT_CONFIRM_PASSWORD_MESSAGE,

	        // Venue Filtering
	        AVAILABLE_COUNTRIES_MESSAGE,
	        SELECT_COUNTRY_MESSAGE,
	        FINAL_VENUE_SELECTION_MESSAGE,
        

    	// Green
    		
	        // System Messages
	        CONFIG_JSON_LOADED_MESSAGE,

       		// Email Config
	        PASSWORD_MATCHES_MESSAGE,
	        SETTINGS_UPDATED_MESSAGE,

	        // Email
	        EMAIL_CREATION_SUCCESS_MESSAGE,
	        CONFIRM_SEND_EMAIL_MESSAGE,
	        CONFIRM_SEND_EMAILS_MESSAGE,
	        EMAILS_SENT_MESSAGE,

	        // Template
		    CONFIRM_SEND_TEMPLATE_MESSAGE,
		    CONFIRM_SEND_TEMPLATES_MESSAGE,


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
	        EMAIL_CONFIG_ENTER_PASSWORD_MESSAGE,
	        EMAIL_CONFIG_CONFIRM_PASSWORD_MESSAGE,
	        SMTP_PASSWORD_CONFIG_MESSAGE,

	        // Email
	        ENTER_SUBJECT_FOR_EMAIL_MESSAGE,
	        ENTER_MESSAGE_FOR_EMAIL_MESSAGE,
	        CONFIRM_ADD_ATTACHMENT_MESSAGE,
	        ADD_ATTACHMENT_MESSAGE,
	        ADD_DIFFERENT_ATTACHMENT_MESSAGE,
	        SMTP_AUTH_MESSAGE,
	        ENTER_CUSTOM_ADDRESS_MESSAGE,


        // Red

	        // Menu
	        CAPS_LOCK_MESSAGE,
	        VENUES_NOT_SELECTED_MESSAGE,
	    	PRESS_RETURN_MESSAGE,
	    	EXIT_CONFIRMATION_MESSAGE,

	        // Email
		    ATTACHMENT_INVALID_CHOICE_MESSAGE,
		    EMAIL_NOT_CREATED_MESSAGE,
		    MODIFY_EMAIL_CONFIRMATION_MESSAGE,
		    EMAIL_SENDING_FAILED_MESSAGE,
		    EMAIL_SENDING_ABORTED_MESSAGE,

		    // Template
		    VENUES_NOT_SELECTED_FOR_TEMPLATES_MESSAGE,
			NO_BOOKING_TEMPLATES_CREATED_MESSAGE,
			MODIFY_TEMPLATE_CONFIRMATION_MESSAGE,
		    MANDATORY_TEMPLATE_FIELD_MESSAGE,
		    EMPTY_FIELD_CONFIRMATION_TEMPLATE_MESSAGE,
		    TEMPLATE_SENDING_ABORTED_MESSAGE
    };

	// Function to handle errors and display appropriate messages based on the type of message
	static void handleMessageAndReturn(MessageType message);

};

#endif // MESSAGEHANDLER_H
