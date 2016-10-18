#include "tldsExtract.h"

TldsExtract *TldsExtract::singleton = nullptr; // singleton instance

TldsExtract* TldsExtract::instance(bool verbose)
{
	// single instance (! one per thread)
	if (!singleton)
		singleton = ::new TldsExtract(verbose);
	return singleton;
}

void TldsExtract::close() // delete the instance
{
	delete singleton;
	singleton = 0;
}

TldsExtract::TldsExtract(bool verbose) : verbose(verbose)
{
	if (verbose)
		std::cout << "Tlds service is loading..." << std::endl;
	loadTlds_(); // should save time
}

TldsExtract::~TldsExtract()
{
	// nothing to do here, instance is killed with TldsExtract::close()
	if (verbose)
		std::cout << "Closing Tlds service." << std::endl;
}

host TldsExtract::extract(std::string hostname) const
{
    host host; // host data structure to return
    host.hostname = hostname;

    // will be used to split hostname in hostpart vector
	std::vector<std::string> hostpart;
	hostpart.push_back(""); // create first string
	size_t stringIndex = 0; // first string index
	std::string::iterator it; //

	// for hostname exemple.act.edu.au, we need =>
	// [0] => exemple, [1] => act, [2] => edu, [3] => au
	for (it = hostname.begin(); it != hostname.end(); it++)
	{
		// FQDN with optionnal final '.' must not change anything
		// (exemple.com. = exemple.com). Here we do not create a new
		// row if it's a final point.
		if ((*it == '.') and (it+1 != hostname.end()))
		{
			hostpart.push_back(""); // create next string
			stringIndex++; // next string index
		}
		else
		{
			hostpart[stringIndex] += *it;
		}
	}
	// reverse
	// [0] => au, [1] => edu, [2] => act, [3] => exemple
	reverse(hostpart.begin(), hostpart.end());

	// string to be find for each suffixes level
	std::string searchString;
	// host depth
	size_t hostdepth = hostpart.size();
	// calculating tld depth to use / -1 = domain should not be in search string
	int depth = (hostdepth > TldsCache::MAX_TLDS_DEPTH) ? TldsCache::MAX_TLDS_DEPTH : hostdepth -1;
	switch (depth)
	{
		case 3 :
				searchString = hostpart[2] + '.' + hostpart[1] + '.' + hostpart[0]; // => act.edu.au
				if (find(tlds.begin(), tlds.end(), searchString) != tlds.end())
				{
                    host.organisation = hostpart[3]; // exemple
                    host.suffix = searchString; // act.edu.au
                	host.tld = hostpart[0]; // au
                    return host;
				}
				/* no break */
		case 2 :
				searchString = hostpart[1] + '.' + hostpart[0]; // => edu.au
				if (find(tlds.begin(), tlds.end(), searchString) != tlds.end())
				{
                    host.organisation = hostpart[2]; // exemple
                    host.suffix = searchString; // edu.au
                	host.tld = hostpart[0]; // au
                    return host;
				}
				/* no break */
		case 1 :
				searchString = hostpart[0]; // => au
				if (find(tlds.begin(), tlds.end(), searchString) != tlds.end())
				{
                    host.organisation = hostpart[1]; // exemple
                    host.suffix = searchString; // au
                	host.tld = hostpart[0]; // au
                    return host;
				}
	}
    // RFC2606, reserved names for a local usage
    // concidered as a TLD if a subdomain exists (myapp.localhost))
    if (hostpart.size() > 1)
    {
        if (find(rfc2606.begin(), rfc2606.end(), hostpart[0]) != rfc2606.end())
        {
            host.organisation = hostpart[1]; // myapp
            host.suffix = hostpart[0]; // localhost
        	host.tld = hostpart[0]; // localhost
            return host;
        }
	}

	// nothing found, only the received host.hostname is to return
    return host;
}

void  TldsExtract::loadTlds_()
{
    try
    {
    	TldsCache(this->tlds, verbose);
        if (verbose)
            std::cout << tlds.size() << " suffixes were loaded (thanks publicsuffix.org)." << std::endl;

    }
    catch (int e)
    {
        if (verbose)
        {
            switch (e)
            {
                case 644:
                    std::cerr << "TLDs cache file is not writable." << std::endl;
                    break;
                case 444:
                    std::cerr << "Suffixes were not loaded, cache file may be corrupt." << std::endl;
                    break;
                case 400:
                    std::cerr << "Curl initialisation failed." << std::endl;
                    break;
                case 404:
                    std::cerr << "Curl connexion to publicsuffix.org failed." << std::endl;
                    break;
            }
        }
        // we continue using RFC2606 (localhost tests)
    }
}
