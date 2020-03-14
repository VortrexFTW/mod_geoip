# gtac_geoip
Maxmind GeoIP module for GTA Connected

**Getting started:**
To use this module, you'll need an MMDB database file from MaxMind: https://www.maxmind.com
You'll need to make an account, get a license key, and then download the database file (it's free)
Upload to your server, and use it in the file arg for the GeoIP functions below. 
Directory starts with the server dir, but relative paths are supported.

**Functions:**
*All functions below return strings.*
```
// Country info
module.geoip.getCountryName(string dbFile, string ipAddress)
module.geoip.getCountryISO(string dbFile, string ipAddress)

// Continent info
module.geoip.getContinentName(string dbFile, string ipAddress)
module.geoip.getContinentCode(string dbFile, string ipAddress)

// For states, provinces, etc
module.geoip.getSubdivisionName(string dbFile, string ipAddress)
module.geoip.getSubdivisionISO(string dbFile, string ipAddress)

// Misc
module.geoip.getCityName(string dbFile, string ipAddress)
module.geoip.getPostalCode(string dbFile, string ipAddress)
```