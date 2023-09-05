#ifndef INIT_H
#define INIT_H

#include "curl.h"
#include "errorhandler.h"
#include "fileutils.h"
#include "filtercriteria.h"
#include "mail.h"
#include "menu.h"

#include <set>
#include <vector>

class Init {
private:
	std::string senderEmail, smtpServer; 
public:

	// Length of asterisks for border
	// ********************************************
	static constexpr int BORDER_LENGTH = 44;

	// Function to display the splashscreen
	static void splashscreen();

	//Function to display the Main Menu to the user
    static void Menu();
};

#endif // INIT_H