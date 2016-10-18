/**
 *	TLDs Cache Service [-std=c++11 -lcurl]
 *
 *	Return cached TLDs list from publicsuffix.org
 *  Curl library is used to download public suffixes
 *  ans create cache.
 *
 *  Thrown exceptions list (int) :
 *	444. Tlds file is empty or corrupt
 *	644. Tlds file is not writable
 *	400. Curl init failed.
 *	404. TLDS were not downloaded (check connexion).
 */
#ifndef TLDSCACHE_H_INCLUDED
#define TLDSCACHE_H_INCLUDED

#include <string>
#include <vector>
#include <fstream>
#include <sstream> // istringstream
#include <bits/stdc++.h> // count string
#include <curl/curl.h> // download public suffix
#include <curl/easy.h> // download public suffix easy !


class TldsCache
{
public:

	// will fill the vector with cached TLDs strings
    TldsCache(std::vector<std::string>& tlds, bool verbose=false);

    // static method to delete cache file content
    static bool deleteCache();

    // max depth to search in TLDs list
	static constexpr const unsigned char MAX_TLDS_DEPTH = 3;
	// cache file name
	static constexpr const char* TLDS_FILE = "suffixes.txt";
	// public suffixes url
	static constexpr const char* TLDS_URL = "https://publicsuffix.org/list/effective_tld_names.dat";

private:

	std::stringstream download_() const; // download public suffix using Curl
	std::vector<std::string>& clean_(std::stringstream stream); // clean downloaded data
	std::vector<std::string>& save_(std::vector<std::string>& tlds) const; // save TLDs
	std::vector<std::string>& load_(); // load TLDs

	std::vector<std::string>& tlds;
	bool verbose;

};

#endif // TLDSCACHE_H_INCLUDED
