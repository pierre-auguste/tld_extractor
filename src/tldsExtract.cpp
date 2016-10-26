#include "tldsExtract.h"


TldsExtract& TldsExtract::instance(bool verbose)
{
	// singleton instance to return
	static TldsExtract instance(verbose);
	return instance;
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

host TldsExtract::extract(std::string const& hostname) const
{
	host host; // host data structure to return
	host.hostname = hostname;

	// FQDN with optionnal final '.' must not change anything
	// (exemple.com. = exemple.com). Here we delete the point.
	if (host.hostname.back() == '.')
		host.hostname.pop_back();

	// for hostname exemple.co.uk, we need =>
	// [0] => uk, [1] => co, [2] => exemple
	std::vector<std::string> hostpart = getHostPart_(host.hostname);

	// SEARCH IN TLD LIST
	// starting from the biggest TLD depth possible
	for (unsigned int i = hostpart.size(); i > 0; i--)
	{
		// string to be find for each suffixes depth
		std::string searchString = hostpart[0]; // to avoid first '.' we set first part...
		for (unsigned int j = 1; j < i; j++) // ... and start from the second vector row
		{
			searchString = hostpart[j] + '.' + searchString; // continue building string
		}

		if (findTld(searchString))
		{
			if (i < hostpart.size()) // that's not only a TLD
				host.organisation = hostpart[i]; // exemple
			host.suffix = searchString; // co.uk
			host.tld = hostpart[0]; // uk
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
		}
	}
	return host;
}

std::vector<std::string> TldsExtract::getHostPart_(std::string const& hostname) const
{
	// will be used to split hostname in hostpart vector
	std::vector<std::string> hostpart;

	hostpart.push_back(""); // create first string
	size_t stringIndex = 0; // first string index

	// for hostname exemple.co.uk
	// [0] => exemple, [1] => co, [2] => uk
	for (char str : hostname)
	{
		if ((str == '.'))
		{
			hostpart.push_back(""); // create next string
			stringIndex++; // next string index
		}
		else
		{
			hostpart[stringIndex] += str;
		}
	}
	// reverse
	// [0] => uk, [1] => co, [2] => exemple
	reverse(hostpart.begin(), hostpart.end());

	return hostpart;
}

bool TldsExtract::findTld(std::string const& str) const
{
	return find(tlds.begin(), tlds.end(), str) != tlds.end();
}

void  TldsExtract::loadTlds_()
{
	try
	{
		TldsCache(this->tlds, verbose); // loading TLD from cache
		if (verbose)
			std::cout << tlds.size() << " suffixes were loaded (thanks publicsuffix.org)." << std::endl;
	}
	catch (int e)
	{
		if (verbose)
		{
			switch (e)
			{
			// FIXME use standart exception
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
