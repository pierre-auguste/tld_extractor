/**
 *  TLDs Extract [-std=c++11 -lcurl]
 *
 *  A singleton class used to extract TLD, organisation and suffix from a hostname
 *  using public suffixes data from Mozilla foundation.
 */
#ifndef TLDSEXTRACT_H_INCLUDED
#define TLDSEXTRACT_H_INCLUDED

#include <iostream>
#include <string>
#include <vector>
#include "tldsCache.h"


/**
 * returned host with extracted TLDs informations
 */
struct host
{
	std::string hostname; // www.exemple.co.uk
	std::string organisation; // exemple
	std::string suffix; // co.uk
	std::string tld; // uk
	std::string domain() const // exemple.co.uk
	{
		return ((not organisation.empty()) and (not suffix.empty()))? organisation + "." + suffix : "" ;
	}
	std::string country() const // uk / ISO 3166.1 alpha-2, TLD country reserved = 2 digit (ex: uk, fr, us, cn, ...)
	{
		return ((not suffix.empty()) and (tld.size() == 2))? tld : "" ;
	}
	std::string subdomain() const // www
	{
		std::string dom = domain();
		return ((not dom.empty()) and (dom != hostname)) ? hostname.substr(0, hostname.find("." + dom)) : "";
	}
};


class TldsExtract // Singleton, usage: host h = TldsExtract::instance()->extract(std::string hostname)
{
public :

	// Return the singleton instance
	static TldsExtract& instance(bool verbose=false);

	// Return a host structure for a given hostname
	host extract(std::string const& hostname);

	// Delete the singleton instance
	void close();

	// return true if the TLD was found
	bool findTld(std::string const& str) const;

private :

	TldsExtract(bool verbose); // private = singleton
	TldsExtract(const TldsExtract&) = delete; // forbid singleton copy (1/3)
	TldsExtract& operator=(const TldsExtract&) = delete; // forbid singleton copy (2/3)
	~TldsExtract(); // forbid singleton copy (optionnal) (3/3)

	std::vector<std::string> getHostPart_(std::string const& hostname);

	void loadTlds_(); // load and set tlds
	std::vector<std::string> tlds; // the TLDs list
	bool verbose;

	// RFC2606, reserved TLD names for a local usage
	// Reserved names will be concidered as a TLD if a subdomain exists (myapp.localhost)
	const std::vector<std::string> rfc2606 = {"test","example","invalid","localhost"};
};


#endif /* TLDSEXTRACT_H_INCLUDED */
