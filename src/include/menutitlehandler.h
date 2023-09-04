#ifndef MENUTITLEHANDLER_H
#define MENUTITLEHANDLER_H

#include "include/fileutils.h"

#include <string>
#include <iostream>

class MenuTitleHandler {

public:
    // Enumeration to represent different types of message that can be encountered
    enum class MenuTitleType {

        // CYAN

            // Main Menu
            MAIN_MENU_HEADER,
            VENUE_SELECTION_MENU_HEADER,
            VENUE_OPTIONS_MENU_HEADER,
            EMAIL_MENU_HEADER,
            TEMPLATES_MENU_HEADER,
            CONFIGURATION_MENU_HEADER,
            EDIT_CONFIGURATION_MENU_HEADER,
            SELECTED_VENUES_MENU_HEADER,

            // Email Menu
            EMAIL_SETTINGS_MENU_HEADER,
            CONSTRUCT_EMAIL_MENU_HEADER,
            EMAIL_DETAILS_MENU_HEADER,
            EMAIL_SAVED_MENU_HEADER,
            TEMPLATE_DETAILS_MENU_HEADER,
            TEMPLATE_SAVED_MENU_HEADER,
            EMAIL_SUMMARY_MENU_HEADER,
            BOOKING_TEMPLATE_SUMMARY_MENU_HEADER,

            // Venue Selection
            VENUE_SELECTION_MENU_HEADER,
            FINAL_VENUE_SELECTION_MENU_HEADER,
            VENUES_ADDED_MENU_HEADER


    };

    static void displayMenuTitle(MenuTitleType menutitle);
};
#endif // end of MENUTITLEHANDLER_H