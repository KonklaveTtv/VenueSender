#include "menutitlehandler.h"

// Use the standard namespace
using namespace std;

void MenuTitleHandler::displayMenuTitle(MenuTitleType menutitle) {

	    switch (menutitle) {


		// Main Menu

		case MenuTitleType::MAIN_MENU_HEADER:
#ifndef UNIT_TESTING
        	ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif	
		        cout << "==========================="<< endl;
		        cout << "         Main Menu         "<< endl;
		        cout << "==========================="<< endl;
#ifndef UNIT_TESTING
        	ConsoleUtils::resetColor();
#endif		    
	    	break;

	    	case MenuTitleType::VENUE_SELECTION_MENU_HEADER:
#ifndef UNIT_TESTING
        	ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
		        cout << "==========================="<< endl;
		        cout << "      Venue Selection      "<< endl;
		        cout << "==========================="<< endl;
#ifndef UNIT_TESTING
        	ConsoleUtils::resetColor();
#endif	
        	break;

		case MenuTitleType::VENUE_OPTIONS_MENU_HEADER:
#ifndef UNIT_TESTING
        	ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
			cout << "==========================="<< endl;
			cout << "       Venue Options       "<< endl;
			cout << "==========================="<< endl;
#ifndef UNIT_TESTING
        	ConsoleUtils::resetColor();
#endif	
	    	break;

		case MenuTitleType::EMAIL_MENU_HEADER:
#ifndef UNIT_TESTING
        	ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
			cout << "==========================="<< endl;
			cout << "           Email           "<< endl;
			cout << "==========================="<< endl;
#ifndef UNIT_TESTING
        	ConsoleUtils::resetColor();
#endif	
	    	break;

		case MenuTitleType::TEMPLATES_MENU_HEADER:
#ifndef UNIT_TESTING
        	ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
			cout << "==========================="<< endl;
			cout << "         Templates         "<< endl;
			cout << "==========================="<< endl;
#ifndef UNIT_TESTING
        	ConsoleUtils::resetColor();
#endif	
	    	break;

		case MenuTitleType::CONFIGURATION_MENU_HEADER:    
#ifndef UNIT_TESTING
        	ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
			cout << "==========================="<< endl;
			cout << "       Configuration       "<< endl;
			cout << "==========================="<< endl;
#ifndef UNIT_TESTING
        	ConsoleUtils::resetColor();
#endif	
		break;

		case MenuTitleType::EDIT_CONFIGURATION_MENU_HEADER:
#ifndef UNIT_TESTING
        	ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
			cout << "===========================" << endl;
			cout << "Edit Configuration Settings" << endl;
			cout << "===========================" << endl;
#ifndef UNIT_TESTING
        	ConsoleUtils::resetColor();
#endif	
		break;

		case MenuTitleType::SELECTED_VENUES_MENU_HEADER:
#ifndef UNIT_TESTING
        	ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
			cout << "==========================="<< endl;
			cout << "      Selected Venues      "<< endl;
			cout << "==========================="<< endl;
#ifndef UNIT_TESTING
        	ConsoleUtils::resetColor();
#endif	
		break;

        // Mail Menus
		case MenuTitleType::EMAIL_SETTINGS_MENU_HEADER:
#ifndef UNIT_TESTING
        	ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
			cout << "============================"<< endl
			     << "       Email Settings       "<< endl
			     << "============================"<< endl;
#ifndef UNIT_TESTING
        	ConsoleUtils::resetColor();
#endif	
		break;

		case MenuTitleType::CONSTRUCT_EMAIL_MENU_HEADER:
#ifndef UNIT_TESTING
        	ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
			cout << "==========================="<< endl;
			cout << "===== Construct Email ====="<< endl;
			cout << "==========================="<< endl;
#ifndef UNIT_TESTING
        	ConsoleUtils::resetColor();
#endif	
		break;

		case MenuTitleType::EMAIL_DETAILS_MENU_HEADER:
#ifndef UNIT_TESTING
        	ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
			cout << "===========================\n";
			cout << "       EMAIL DETAILS       \n";
			cout << "===========================\n";
#ifndef UNIT_TESTING
        	ConsoleUtils::resetColor();
#endif	
		break;

		case MenuTitleType::EMAIL_SAVED_MENU_HEADER:
#ifndef UNIT_TESTING
        	ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
			cout << "===========================" << endl;
			cout << "        Email saved        " << endl;
			cout << "===========================" << endl;
#ifndef UNIT_TESTING
        	ConsoleUtils::resetColor();
#endif	
		break;

		case MenuTitleType::TEMPLATE_DETAILS_MENU_HEADER:
#ifndef UNIT_TESTING
        	ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
			cout << "=============================\n";
			cout << "      TEMPLATE DETAILS       \n";
			cout << "=============================\n";
#ifndef UNIT_TESTING
        	ConsoleUtils::resetColor();
#endif	
		break;

		case MenuTitleType::TEMPLATE_SAVED_MENU_HEADER:
#ifndef UNIT_TESTING
        	ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
			cout << "===========================" << endl;
			cout << "      Template Saved       " << endl;
			cout << "===========================" << endl;
#ifndef UNIT_TESTING
        	ConsoleUtils::resetColor();
#endif	
		break;

		case MenuTitleType::EMAIL_SUMMARY_MENU_HEADER:
#ifndef UNIT_TESTING
        	ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
			cout << "===========================" << endl;
			cout << "       Email Summary       " << endl;
			cout << "==========================="<< endl;
#ifndef UNIT_TESTING
        	ConsoleUtils::resetColor();
#endif	
		break;

		case MenuTitleType::BOOKING_TEMPLATE_SUMMARY_MENU_HEADER:
#ifndef UNIT_TESTING
        	ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
			cout << "============================"<< endl;
			cout << "  Booking Template Summary  "<< endl;
			cout << "============================"<< endl;
#ifndef UNIT_TESTING
        	ConsoleUtils::resetColor();
#endif	
		break;


        // Filter Menus

        // File Utils



    }
#ifndef UNIT_TESTING
        ConsoleUtils::resetColor();
#endif
}
