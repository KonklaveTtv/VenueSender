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
    int smtpPort;
    bool useSSL, verifyPeer, verifyHost, verbose;
public:
	// Function to display the splashscreen
	static void splashscreen();

	//Function to display the Main Menu to the user
    void Menu();
};

#endif // INIT_H