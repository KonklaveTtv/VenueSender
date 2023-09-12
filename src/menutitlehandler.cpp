#include "menutitlehandler.h"

// Use the standard namespace
using namespace std;

void MenuTitleHandler::displayMenuTitle(MenuTitleType menutitle) {

    	switch (menutitle) {

	// CYAN

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
			cout << "      Construct Email      "<< endl;
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

		case MenuTitleType::CYAN_BORDER:
#ifndef UNIT_TESTING
		ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif		
		cout << "==========================="<< endl;
#ifndef UNIT_TESTING
		ConsoleUtils::resetColor();
#endif		
		break;


        // Filter Menus
		case MenuTitleType::FINAL_VENUE_SELECTION_MENU_HEADER:
#ifndef UNIT_TESTING
        	ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
			cout << "===========================" << endl;
			cout << "   Final Venue Selection   " << endl;
			cout << "===========================\n";
#ifndef UNIT_TESTING
        	ConsoleUtils::resetColor();
#endif	
		break;	

		case MenuTitleType::VENUES_ADDED_MENU_HEADER:
#ifndef UNIT_TESTING
        	ConsoleUtils::setColor(ConsoleUtils::Color::CYAN);
#endif
		cout << "===========================" << endl;
		cout << "        Venues Added       " << endl;
#ifndef UNIT_TESTING
        	ConsoleUtils::resetColor();
#endif	
		break;	
	// end of CYAN


	// LIGHT BLUE

		// Email Menu

		case MenuTitleType::LIGHT_BLUE_BORDER:
#ifndef UNIT_TESTING
		ConsoleUtils::setColor(ConsoleUtils::Color::LIGHT_BLUE);
#endif		
		cout << "==========================="<< endl;
#ifndef UNIT_TESTING
		ConsoleUtils::resetColor();
#endif		
		break;
	// end of LIGHT BLUE

        // ORANGE

        	// Email Menu

		case MenuTitleType::ORANGE_BORDER:
#ifndef UNIT_TESTING
		ConsoleUtils::setColor(ConsoleUtils::Color::ORANGE);
#endif		
		cout << "==========================="<< endl;
#ifndef UNIT_TESTING
		ConsoleUtils::resetColor();
#endif		
		break;
	// end of ORANGE

		// GREEN

			// Email Menu

		case MenuTitleType::EMAIL_SAVED_MENU_HEADER:
#ifndef UNIT_TESTING
        	ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
			cout << "===========================" << endl;
			cout << "        Email saved        " << endl;
			cout << "===========================" << endl;
#ifndef UNIT_TESTING
        	ConsoleUtils::resetColor();
#endif	
		break;

		case MenuTitleType::TEMPLATE_SAVED_MENU_HEADER:
#ifndef UNIT_TESTING
        	ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif
			cout << "===========================" << endl;
			cout << "      Template Saved       " << endl;
			cout << "===========================" << endl;
#ifndef UNIT_TESTING
        	ConsoleUtils::resetColor();
#endif	
		break;

		case MenuTitleType::GREEN_BORDER:
#ifndef UNIT_TESTING
		ConsoleUtils::setColor(ConsoleUtils::Color::GREEN);
#endif		
		cout << "==========================="<< endl;
#ifndef UNIT_TESTING
		ConsoleUtils::resetColor();
#endif		
		break;
	// end of GREEN

		// RED

	        // Default Menu Item
#ifdef UNIT_TESTING
	        default:
	            cerr << "" << endl;
	            break;
#else
	        default:
#ifndef UNIT_TESTING
		ConsoleUtils::setColor(ConsoleUtils::Color::RED);
#endif	
	            cerr << "UNKNOWN MENU ITEM" << endl;
#ifndef UNIT_TESTING
		ConsoleUtils::resetColor();
#endif
	            break;
#endif
    }
}