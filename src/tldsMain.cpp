/**
 * 	tldExtractor [-std=c++11 -lcurl]
 *
 * 	An application to extract organisation and suffixes from an URL.
 *
 * 	Read https://publicsuffix.org/learn/ if you want to improve your knowledge
 * 	about TLD and suffixes.
 */
#include <iostream>
#include <string>
#include <vector>
#include "tldsExtract.h"
#include "tldsCache.h"

static void show_usage(std::string name)
{
    std::cout //<< std::endl
	<< "An application to extract suffixes and organisation from an URL." << std::endl
	<< std::endl
	<< "Usage :" << std::endl
	<< "   " << name << " <options> [URL]" << std::endl
	<< std::endl
	<< "Options :" << std::endl
	<< "   -h, --help         Show this help message." << std::endl
	<< "   -u, --update       Force TLD cache to be updated." << std::endl
	<< "   -i, --interactive  Interactive mode (usefull for debugging)." << std::endl
	<< "   -f, --hostname     Return full hostname from [URL]." << std::endl
	<< "   -o, --organisation Return organisation from [URL]." << std::endl
	<< "   -s, --suffix       Return suffix(es) from [URL]." << std::endl
	<< "   -p, --subdomain    Return subdomain from [URL]." << std::endl
	<< "   -d, --domain       Return domain (organisation + suffix) from [URL]." << std::endl
	<< "   -t, --tld          Return TLD from [URL]." << std::endl
	<< "   -c, --country      Return country from [URL]." << std::endl
	<< std::endl
	<< "Note :" << std::endl
	<< "   If multiple data are to be returned, a slash will be used to" << std::endl
	<< "   separate them." << std::endl
	<<     std::endl
	<< "   Maximum depth of suffixes search was fixed to three." << std::endl
	<< "   Exemple: *.act.edu.au" << std::endl
	<<     std::endl
	<< "   A cache file named \"" << TldsCache::TLDS_FILE << "\" and containing" << std::endl
	<< "   suffixes is created on your local system." << std::endl
	<<     std::endl
	<< "   RFC2606: Reserved names are returned as a TLD if a subdomain exists." << std::endl
	<< "   Exemple: myapp.localhost" << std::endl
	<< "   Reserved names are: test, example, invalid, localhost." << std::endl
	<< std::endl
	<< "Exemples of usage:" << std::endl
	<< "   Delete cache:" << std::endl
	<< "    $ " << name << " -u" << std::endl
	<< "   Delete cache and enter interactive mode" << std::endl
	<< "    $ " << name << " -u -i" << std::endl
	<< "   Get all informations about www.exemple.co.uk" << std::endl
	<< "    $ " << name << " -f -o -s -p -d -t -c http://www.exemple.co.uk/index.html" << std::endl
	<< std::endl;
}

// type of option selected (-f, -o, -s, -p, -d, -t, -c)
enum search {hostname, organisation, suffix, subdomain, domain, tld, country};

// extract hostname from an URL
std::string getHostnameFromUrl(std::string& url);
// this enter interactive mode
bool interactive();
// for non-interactive mode
bool extract(std::vector<search> searches, std::string url);


int main(int argc, char *argv[])
{
	if (argc < 2) // We need minimum program name and one argument
	{
		show_usage(argv[0]);
		return EXIT_FAILURE;
	}

	bool deleteCache = false; // if true will delete cache
	bool interactiveMode = false; // if true will turn application to interactiv mod
	std::vector<search> searches; // search type  (-f, -o, -s, ...)

	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];
		if ((arg == "-h") || (arg == "--help"))
		{
			show_usage(argv[0]);
			return EXIT_SUCCESS;
		}
		else if ((arg == "-u") || (arg == "--update"))
		{
			deleteCache = true;
		}
		else if ((arg == "-i") || (arg == "--interactive"))
		{
			interactiveMode = true;
		}
		else if ((arg == "-f") || (arg == "--hostname"))
		{
			searches.push_back(hostname);
		}
		else if ((arg == "-o") || (arg == "--organisation"))
		{
			searches.push_back(organisation);
		}
		else if ((arg == "-s") || (arg == "--suffix"))
		{
			searches.push_back(suffix);
		}
		else if ((arg == "-p") || (arg == "--subdomain"))
		{
			searches.push_back(subdomain);
		}
		else if ((arg == "-d") || (arg == "--domain"))
		{
			searches.push_back(domain);
		}
		else if ((arg == "-t") || (arg == "--tld"))
		{
			searches.push_back(tld);
		}
		else if ((arg == "-c") || (arg == "--country"))
		{
			searches.push_back(country);
		}
		// For these options we need an URL
		if ((arg == "-f") || (arg == "--hostname")
		||  (arg == "-o") || (arg == "--organisation")
		||  (arg == "-s") || (arg == "--suffix")
		||  (arg == "-p") || (arg == "--subdomain")
		||  (arg == "-d") || (arg == "--domain")
		||  (arg == "-t") || (arg == "--tld")
		||  (arg == "-c") || (arg == "--country"))
		{
			if (i + 1 >= argc) // has argc enough arguments
			{
				std::cerr << "this option requires an [URL]." << std::endl;
				return EXIT_FAILURE;
			}
		}
    }

    if (deleteCache)
    {
    	TldsCache::deleteCache();
    	if (argc == 2)
    		return EXIT_SUCCESS;
    }

    if (interactiveMode)
    {
    	return interactive();
    }

    if (searches.size() > 0)
    {
    	std::string url = argv[argc-1];
    	if (url.at(0) != '-') // is url a badly set option ? (ex. "-d -u")
            return extract(searches, url);
    }

    show_usage(argv[0]);
    return EXIT_FAILURE;
}

std::string getHostnameFromUrl(std::string& url)
{
	size_t pos;

	// deleting URL scheme
	// we delete "://" and everything before
	pos = url.find("://");
	if (pos != std::string::npos)
		url.erase(0, pos + 3);

	// deleting URL port
	// we delete first ":" and everything after
	pos = url.find(":");
	if (pos != std::string::npos)
		url.erase(pos);

	// deleting URL path and query
	// as scheme is deleted, we now delete first "/" and everything after
	pos = url.find("/");
	if (pos != std::string::npos)
		url.erase(pos);

	return url;
}

bool extract(std::vector<search> searches, std::string url)
{
	host h = TldsExtract::instance()->extract(getHostnameFromUrl(url));
	TldsExtract::instance()->close(); // clean close of TLD services

	bool first = true;
	for (search type: searches)
	{
		if (not first)
			std::cout << "/";
		first = false;

		switch (type)
		{
			case hostname :
				std::cout << h.hostname;
				break;
			case organisation :
				std::cout << h.organisation;
				break;
			case suffix :
				std::cout << h.suffix;
				break;
			case subdomain :
				std::cout << h.subdomain();
				break;
			case domain :
				std::cout << h.domain();
				break;
			case tld :
				std::cout << h.tld;
				break;
			case country :
				std::cout << h.country();
				break;
		}
	}
	std::cout << std::endl;
	return EXIT_SUCCESS;
}

bool interactive()
{
    std::string url; // user url
    host host; // returned structured host

    while(true)
    {
        std::cout << "Please, enter a url (or \"bye\" to exit) :" << std::endl;
        std::cin >> url;

        if (url == "bye")
        {
            if (not host.hostname.empty()) // a tld instance was previously launched
                TldsExtract::instance(true)->close(); // clean close of TLD services
            std::cout << "Bye bye, have fun !" << std::endl;
            return EXIT_SUCCESS;
        }

        host = TldsExtract::instance(true)->extract(getHostnameFromUrl(url));

        std::cout << "----------------------------------------------------" << std::endl;
        std::cout << "Hostname:     " << host.hostname << std::endl;
        std::cout << "Organisation: " << host.organisation << std::endl;
        std::cout << "Suffix:       " << host.suffix << std::endl;
        std::cout << "Subdomain:    " << host.subdomain() << std::endl;
        std::cout << "Domain:       " << host.domain() << std::endl;
        std::cout << "TLD:          " << host.tld << std::endl;
        std::cout << "Country:      " << host.country() << std::endl;
        std::cout << "----------------------------------------------------" << std::endl;
    }
}
