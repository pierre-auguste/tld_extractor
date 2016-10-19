# tld_extractor
C++11 application to extract suffixes and organisation from an URL.
Read https://publicsuffix.org/learn/ if you want to improve your knowledge about TLD and suffixes.
 Use curl library to download public suffix

Usage :

   ./tldExtractor <options> [URL]

Options :
   -h, --help         Show this help message.
   -u, --update       Force TLD cache to be updated.
   -i, --interactive  Interactive mode (usefull for debugging).
   -f, --hostname     Return full hostname from [URL].
   -o, --organisation Return organisation from [URL].
   -s, --suffix       Return suffix(es) from [URL].
   -p, --subdomain    Return subdomain from [URL].
   -d, --domain       Return domain (organisation + suffix) from [URL].
   -t, --tld          Return TLD from [URL].
   -c, --country      Return country from [URL].

Note :

   If multiple data are to be returned, a slash will be used to
   separate them.

   Maximum depth of suffixes search was fixed to three.
   Exemple: *.act.edu.au

   A cache file named "suffixes.txt" and containing
   suffixes is created on your local system.

   RFC2606: Reserved names are returned as a TLD if a subdomain exists.
   Exemple: myapp.localhost
   Reserved names are: test, example, invalid, localhost.

Exemples of usage:

   Delete cache:
    $ ./tldExtractor -u
   Delete cache and enter interactive mode
    $ ./tldExtractor -u -i
   Get all informations about www.exemple.co.uk
    $ ./tldExtractor -f -o -s -p -d -t -c http://www.exemple.co.uk/index.html

