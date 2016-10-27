#include "tldsCache.h"

TldsCache::TldsCache(std::vector<std::string>& tlds, bool verbose) :
					 tlds(tlds), // the vector to fill
					 verbose(verbose)
{
	// testing file
	std::ifstream ifsTlds(TLDS_FILE);
	if (not ifsTlds || ifsTlds.peek() == std::ifstream::traits_type::eof())
	{
		if (this->verbose)
			std::cout << "Downloading Tlds, please wait..." << std::endl;
		// running the long way...
		this->tlds = save_(clean_(download_()));
	}
	else
	{
		// running the short way...
		this->tlds = load_();
	}
}

bool TldsCache::deleteCache()
{
	// delete file content but keep the file
	std::ofstream ofsTlds(TLDS_FILE, std::ofstream::out | std::ofstream::trunc);
	return static_cast<bool>(ofsTlds); // thanks Ksass`Peuk for the correction
}

std::vector<std::string>& TldsCache::load_()
{
	// loading TLDs from file
	std::ifstream ifsTlds(TLDS_FILE);
	std::string line;
	while (getline(ifsTlds, line))
		tlds.push_back(line);

	// we should have more than 8000 TLDs
	size_t minimum = 8000;
	if (tlds.size() < minimum)
		throw 444; // File is corrupt

	return tlds; // for programmatic logic
}

std::vector<std::string>& TldsCache::save_(std::vector<std::string>& tlds) const
{
	// testing file
	std::ofstream ofsTlds(TLDS_FILE);
	if (not ofsTlds)
		throw 644; // File is not writable

	// saving TLDs to file
	for (std::string tld : tlds)
		ofsTlds << tld << std::endl;

	return tlds; // for programmatic logic
}

std::vector<std::string>& TldsCache::clean_(std::stringstream readBuffer)
{
	std::string line;
	while (getline(readBuffer, line))
	{
		// filtering TLDS and filling the vector
		if ((not line.empty()) // no empty line
		 && (line.at(0) != '/')) // no comment
			tlds.push_back(line);
	}
	return tlds; // for programmatic logic
}

/**
 * char *contents = data received from Curl
 * void *readBuffer = stringstream buffer, @see CURLOPT_WRITEDATA in Tlds::_downloadTlds()
 */
static size_t CurlToStringstreamWriterCallback(char *contents, size_t size, size_t nmemb, void *readBuffer)
{
	size_t realsize = size * nmemb;
	((std::stringstream*)readBuffer)->write((char *)contents , realsize);
	return realsize;
}

std::stringstream TldsCache::download_() const
{
	std::stringstream readBuffer; // downloaded TLDs to return

	CURL *curl = curl_easy_init(); // see Curl documentation
	if (not curl)
		throw 400; // Curl init failed

	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1); // Use it to debug
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlToStringstreamWriterCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
	curl_easy_setopt(curl, CURLOPT_URL, TLDS_URL);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

	CURLcode response = curl_easy_perform(curl);
	if(response != CURLE_OK)
	{
		curl_easy_cleanup(curl);
		if (verbose)
			std::cerr << curl_easy_strerror(response) << std::endl;
		throw 404; // Curl connexion failed
	}
	curl_easy_cleanup(curl);
	return readBuffer;
}
