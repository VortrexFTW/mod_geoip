#include <stdio.h>
#include <stdlib.h>

#include <SDKHelper.h>
#include <memory>

#include <maxminddb.h>

#ifndef _WIN32
#define sprintf_s snprintf
#endif

#ifndef MMDB_UINT128_USING_MODE
/* Define as 1 if we we use unsigned int __atribute__ ((__mode__(TI))) for uint128 values */
#define MMDB_UINT128_USING_MODE 0
#endif

#ifndef MMDB_UINT128_IS_BYTE_ARRAY
/* Define as 1 if we don't have an unsigned __int128 type */
#undef MMDB_UINT128_IS_BYTE_ARRAY
#endif

// The modules internal name (Also used for the namespace name)
MODULE_MAIN("geoip");

void ModuleRegister()
{
	SDK::RegisterFunction("getCountryName", [](Galactic3D::Interfaces::INativeState* pState, int32_t argc, void* pUser) {
		SDK_TRY;

		SDK::State State(pState);
		
		const char* szFileName = State.CheckString(0);

		MMDB_s* pMMDB = (MMDB_s*)malloc(sizeof(MMDB_s));
		if(pMMDB == NULL) {
			return pState->SetError("%s", MMDB_strerror(MMDB_OUT_OF_MEMORY_ERROR));
		}

		int iMMDBStatus = MMDB_open(szFileName, MMDB_MODE_MMAP, pMMDB);
		if(iMMDBStatus != MMDB_SUCCESS) {
			return pState->SetError("Could not load database file %s", szFileName);
		}

		const char* szIP = State.CheckString(1);

		int iGaiError = 0;
		int iMMDBError = MMDB_SUCCESS;
		MMDB_lookup_result_s pResult = MMDB_lookup_string(pMMDB, szIP, &iGaiError, &iMMDBError);

		if(iGaiError != 0) {
			return pState->SetError("%s does not appear to be an IPv4 or IPv6 address.", szIP);
		}

		if(iMMDBError != MMDB_SUCCESS) {
			return pState->SetError("Failed lookup for %s (%s)", szIP, MMDB_strerror(iMMDBError));
		}

		if(!pResult.found_entry) {
			return pState->SetError("No results found for %s", szIP);
		}

		MMDB_entry_data_s pEntryData;
		int iLookupStatus = MMDB_get_value(&pResult.entry, &pEntryData, "country", "names", "en", NULL);

		if(iLookupStatus != MMDB_SUCCESS) {
			return pState->SetError("Failed to get data for %s (%s)", szIP, MMDB_strerror(iLookupStatus));
		}

		if(!pEntryData.has_data) {
			return pState->SetError("Failed to get data for %s (no data returned)", szIP);
		}
		
		if(pEntryData.type != MMDB_DATA_TYPE_UTF8_STRING) {
			return pState->SetError("Failed to parse data for %s", szIP);
		}
		
		SDK::StringValue Value(pEntryData.utf8_string, pEntryData.data_size);		
		State.Return(Value);
	
		MMDB_close(pMMDB);
		return true;

		SDK_ENDTRY;
	});

	SDK::RegisterFunction("getCountryISO", [](Galactic3D::Interfaces::INativeState* pState, int32_t argc, void* pUser) {
		SDK_TRY;

		SDK::State State(pState);
		
		const char* szFileName = State.CheckString(0);

		MMDB_s* pMMDB = (MMDB_s*)malloc(sizeof(MMDB_s));
		if(pMMDB == NULL) {
			return pState->SetError("%s", MMDB_strerror(MMDB_OUT_OF_MEMORY_ERROR));
		}

		int iMMDBStatus = MMDB_open(szFileName, MMDB_MODE_MMAP, pMMDB);
		if(iMMDBStatus != MMDB_SUCCESS) {
			return pState->SetError("Could not load database file %s", szFileName);
		}

		const char* szIP = State.CheckString(1);

		int iGaiError = 0;
		int iMMDBError = MMDB_SUCCESS;
		MMDB_lookup_result_s pResult = MMDB_lookup_string(pMMDB, szIP, &iGaiError, &iMMDBError);

		if(iGaiError != 0) {
			return pState->SetError("%s does not appear to be an IPv4 or IPv6 address.", szIP);
		}

		if(iMMDBError != MMDB_SUCCESS) {
			return pState->SetError("Failed lookup for %s (%s)", szIP, MMDB_strerror(iMMDBError));
		}

		if(!pResult.found_entry) {
			return pState->SetError("No results found for %s", szIP);
		}

		MMDB_entry_data_s pEntryData;
		int iLookupStatus = MMDB_get_value(&pResult.entry, &pEntryData, "country", "iso_code", NULL);

		if(iLookupStatus != MMDB_SUCCESS) {
			return pState->SetError("Failed to get data for %s (%s)", szIP, MMDB_strerror(iLookupStatus));
		}

		if(!pEntryData.has_data) {
			return pState->SetError("Failed to get data for %s (no data returned)", szIP);
		}
		
		if(pEntryData.type != MMDB_DATA_TYPE_UTF8_STRING) {
			return pState->SetError("Failed to parse data for %s", szIP);
		}
		
		SDK::StringValue Value(pEntryData.utf8_string, pEntryData.data_size);		
		State.Return(Value);
	
		MMDB_close(pMMDB);
		return true;

		SDK_ENDTRY;
	});
	
	SDK::RegisterFunction("getContinentName", [](Galactic3D::Interfaces::INativeState* pState, int32_t argc, void* pUser) {
		SDK_TRY;

		SDK::State State(pState);
		
		const char* szFileName = State.CheckString(0);

		MMDB_s* pMMDB = (MMDB_s*)malloc(sizeof(MMDB_s));
		if(pMMDB == NULL) {
			return pState->SetError("%s", MMDB_strerror(MMDB_OUT_OF_MEMORY_ERROR));
		}

		int iMMDBStatus = MMDB_open(szFileName, MMDB_MODE_MMAP, pMMDB);
		if(iMMDBStatus != MMDB_SUCCESS) {
			return pState->SetError("Could not load database file %s", szFileName);
		}

		const char* szIP = State.CheckString(1);

		int iGaiError = 0;
		int iMMDBError = MMDB_SUCCESS;
		MMDB_lookup_result_s pResult = MMDB_lookup_string(pMMDB, szIP, &iGaiError, &iMMDBError);

		if(iGaiError != 0) {
			return pState->SetError("%s does not appear to be an IPv4 or IPv6 address.", szIP);
		}

		if(iMMDBError != MMDB_SUCCESS) {
			return pState->SetError("Failed lookup for %s (%s)", szIP, MMDB_strerror(iMMDBError));
		}

		if(!pResult.found_entry) {
			return pState->SetError("No results found for %s", szIP);
		}

		MMDB_entry_data_s pEntryData;
		int iLookupStatus = MMDB_get_value(&pResult.entry, &pEntryData, "continent", "names", "en", NULL);

		if(iLookupStatus != MMDB_SUCCESS) {
			return pState->SetError("Failed to get data for %s (%s)", szIP, MMDB_strerror(iLookupStatus));
		}

		if(!pEntryData.has_data) {
			return pState->SetError("Failed to get data for %s (no data returned)", szIP);
		}
		
		if(pEntryData.type != MMDB_DATA_TYPE_UTF8_STRING) {
			return pState->SetError("Failed to parse data for %s", szIP);
		}
		
		SDK::StringValue Value(pEntryData.utf8_string, pEntryData.data_size);		
		State.Return(Value);
	
		MMDB_close(pMMDB);
		return true;

		SDK_ENDTRY;
	});	

	SDK::RegisterFunction("getContinentCode", [](Galactic3D::Interfaces::INativeState* pState, int32_t argc, void* pUser) {
		SDK_TRY;

		SDK::State State(pState);
		
		const char* szFileName = State.CheckString(0);

		MMDB_s* pMMDB = (MMDB_s*)malloc(sizeof(MMDB_s));
		if(pMMDB == NULL) {
			return pState->SetError("%s", MMDB_strerror(MMDB_OUT_OF_MEMORY_ERROR));
		}

		int iMMDBStatus = MMDB_open(szFileName, MMDB_MODE_MMAP, pMMDB);
		if(iMMDBStatus != MMDB_SUCCESS) {
			return pState->SetError("Could not load database file %s", szFileName);
		}

		const char* szIP = State.CheckString(1);

		int iGaiError = 0;
		int iMMDBError = MMDB_SUCCESS;
		MMDB_lookup_result_s pResult = MMDB_lookup_string(pMMDB, szIP, &iGaiError, &iMMDBError);

		if(iGaiError != 0) {
			return pState->SetError("%s does not appear to be an IPv4 or IPv6 address.", szIP);
		}

		if(iMMDBError != MMDB_SUCCESS) {
			return pState->SetError("Failed lookup for %s (%s)", szIP, MMDB_strerror(iMMDBError));
		}

		if(!pResult.found_entry) {
			return pState->SetError("No results found for %s", szIP);
		}

		MMDB_entry_data_s pEntryData;
		int iLookupStatus = MMDB_get_value(&pResult.entry, &pEntryData, "continent", "code", NULL);

		if(iLookupStatus != MMDB_SUCCESS) {
			return pState->SetError("Failed to get data for %s (%s)", szIP, MMDB_strerror(iLookupStatus));
		}

		if(!pEntryData.has_data) {
			return pState->SetError("Failed to get data for %s (no data returned)", szIP);
		}
		
		if(pEntryData.type != MMDB_DATA_TYPE_UTF8_STRING) {
			return pState->SetError("Failed to parse data for %s", szIP);
		}
		
		SDK::StringValue Value(pEntryData.utf8_string, pEntryData.data_size);		
		State.Return(Value);
	
		MMDB_close(pMMDB);
		return true;

		SDK_ENDTRY;
	});		

	SDK::RegisterFunction("getCityName", [](Galactic3D::Interfaces::INativeState* pState, int32_t argc, void* pUser) {
		SDK_TRY;

		SDK::State State(pState);
		
		const char* szFileName = State.CheckString(0);

		MMDB_s* pMMDB = (MMDB_s*)malloc(sizeof(MMDB_s));
		if(pMMDB == NULL) {
			return pState->SetError("%s", MMDB_strerror(MMDB_OUT_OF_MEMORY_ERROR));
		}

		int iMMDBStatus = MMDB_open(szFileName, MMDB_MODE_MMAP, pMMDB);
		if(iMMDBStatus != MMDB_SUCCESS) {
			return pState->SetError("Could not load database file %s", szFileName);
		}

		const char* szIP = State.CheckString(1);

		int iGaiError = 0;
		int iMMDBError = MMDB_SUCCESS;
		MMDB_lookup_result_s pResult = MMDB_lookup_string(pMMDB, szIP, &iGaiError, &iMMDBError);

		if(iGaiError != 0) {
			return pState->SetError("%s does not appear to be an IPv4 or IPv6 address.", szIP);
		}

		if(iMMDBError != MMDB_SUCCESS) {
			return pState->SetError("Failed lookup for %s (%s)", szIP, MMDB_strerror(iMMDBError));
		}

		if(!pResult.found_entry) {
			return pState->SetError("No results found for %s", szIP);
		}

		MMDB_entry_data_s pEntryData;
		int iLookupStatus = MMDB_get_value(&pResult.entry, &pEntryData, "city", "names", "en", NULL);

		if(iLookupStatus != MMDB_SUCCESS) {
			return pState->SetError("Failed to get data for %s (%s)", szIP, MMDB_strerror(iLookupStatus));
		}

		if(!pEntryData.has_data) {
			return pState->SetError("Failed to get data for %s (no data returned)", szIP);
		}
		
		if(pEntryData.type != MMDB_DATA_TYPE_UTF8_STRING) {
			return pState->SetError("Failed to parse data for %s", szIP);
		}
		
		SDK::StringValue Value(pEntryData.utf8_string, pEntryData.data_size);		
		State.Return(Value);
	
		MMDB_close(pMMDB);
		return true;

		SDK_ENDTRY;
	});
	
	SDK::RegisterFunction("getPostalCode", [](Galactic3D::Interfaces::INativeState* pState, int32_t argc, void* pUser) {
		SDK_TRY;

		SDK::State State(pState);
		
		const char* szFileName = State.CheckString(0);

		MMDB_s* pMMDB = (MMDB_s*)malloc(sizeof(MMDB_s));
		if(pMMDB == NULL) {
			return pState->SetError("%s", MMDB_strerror(MMDB_OUT_OF_MEMORY_ERROR));
		}

		int iMMDBStatus = MMDB_open(szFileName, MMDB_MODE_MMAP, pMMDB);
		if(iMMDBStatus != MMDB_SUCCESS) {
			return pState->SetError("Could not load database file %s", szFileName);
		}

		const char* szIP = State.CheckString(1);

		int iGaiError = 0;
		int iMMDBError = MMDB_SUCCESS;
		MMDB_lookup_result_s pResult = MMDB_lookup_string(pMMDB, szIP, &iGaiError, &iMMDBError);

		if(iGaiError != 0) {
			return pState->SetError("%s does not appear to be an IPv4 or IPv6 address.", szIP);
		}

		if(iMMDBError != MMDB_SUCCESS) {
			return pState->SetError("Failed lookup for %s (%s)", szIP, MMDB_strerror(iMMDBError));
		}

		if(!pResult.found_entry) {
			return pState->SetError("No results found for %s", szIP);
		}

		MMDB_entry_data_s pEntryData;
		int iLookupStatus = MMDB_get_value(&pResult.entry, &pEntryData, "postal", "code", NULL);

		if(iLookupStatus != MMDB_SUCCESS) {
			return pState->SetError("Failed to get data for %s (%s)", szIP, MMDB_strerror(iLookupStatus));
		}

		if(!pEntryData.has_data) {
			return pState->SetError("Failed to get data for %s (no data returned)", szIP);
		}
		
		if(pEntryData.type != MMDB_DATA_TYPE_UTF8_STRING) {
			return pState->SetError("Failed to parse data for %s", szIP);
		}
		
		SDK::StringValue Value(pEntryData.utf8_string, pEntryData.data_size);		
		State.Return(Value);
	
		MMDB_close(pMMDB);
		return true;

		SDK_ENDTRY;
	});	

	SDK::RegisterFunction("getSubdivisionName", [](Galactic3D::Interfaces::INativeState* pState, int32_t argc, void* pUser) {
		SDK_TRY;

		SDK::State State(pState);
		
		const char* szFileName = State.CheckString(0);

		MMDB_s* pMMDB = (MMDB_s*)malloc(sizeof(MMDB_s));
		if(pMMDB == NULL) {
			return pState->SetError("%s", MMDB_strerror(MMDB_OUT_OF_MEMORY_ERROR));
		}

		int iMMDBStatus = MMDB_open(szFileName, MMDB_MODE_MMAP, pMMDB);
		if(iMMDBStatus != MMDB_SUCCESS) {
			return pState->SetError("Could not load database file %s", szFileName);
		}

		const char* szIP = State.CheckString(1);

		int iGaiError = 0;
		int iMMDBError = MMDB_SUCCESS;
		MMDB_lookup_result_s pResult = MMDB_lookup_string(pMMDB, szIP, &iGaiError, &iMMDBError);

		if(iGaiError != 0) {
			return pState->SetError("%s does not appear to be an IPv4 or IPv6 address.", szIP);
		}

		if(iMMDBError != MMDB_SUCCESS) {
			return pState->SetError("Failed lookup for %s (%s)", szIP, MMDB_strerror(iMMDBError));
		}

		if(!pResult.found_entry) {
			return pState->SetError("No results found for %s", szIP);
		}

		MMDB_entry_data_s pEntryData;
		int iLookupStatus = MMDB_get_value(&pResult.entry, &pEntryData, "subdivisions", "0", "names", "en", NULL);

		if(iLookupStatus != MMDB_SUCCESS) {
			return pState->SetError("Failed to get data for %s (%s)", szIP, MMDB_strerror(iLookupStatus));
		}

		if(!pEntryData.has_data) {
			return pState->SetError("Failed to get data for %s (no data returned)", szIP);
		}
		
		if(pEntryData.type != MMDB_DATA_TYPE_UTF8_STRING) {
			return pState->SetError("Failed to parse data for %s", szIP);
		}
		
		SDK::StringValue Value(pEntryData.utf8_string, pEntryData.data_size);		
		State.Return(Value);
	
		MMDB_close(pMMDB);
		return true;

		SDK_ENDTRY;
	});	
	
	SDK::RegisterFunction("getSubdivisionISO", [](Galactic3D::Interfaces::INativeState* pState, int32_t argc, void* pUser) {
		SDK_TRY;

		SDK::State State(pState);
		
		const char* szFileName = State.CheckString(0);

		MMDB_s* pMMDB = (MMDB_s*)malloc(sizeof(MMDB_s));
		if(pMMDB == NULL) {
			return pState->SetError("%s", MMDB_strerror(MMDB_OUT_OF_MEMORY_ERROR));
		}

		int iMMDBStatus = MMDB_open(szFileName, MMDB_MODE_MMAP, pMMDB);
		if(iMMDBStatus != MMDB_SUCCESS) {
			return pState->SetError("Could not load database file %s", szFileName);
		}

		const char* szIP = State.CheckString(1);

		int iGaiError = 0;
		int iMMDBError = MMDB_SUCCESS;
		MMDB_lookup_result_s pResult = MMDB_lookup_string(pMMDB, szIP, &iGaiError, &iMMDBError);

		if(iGaiError != 0) {
			return pState->SetError("%s does not appear to be an IPv4 or IPv6 address.", szIP);
		}

		if(iMMDBError != MMDB_SUCCESS) {
			return pState->SetError("Failed lookup for %s (%s)", szIP, MMDB_strerror(iMMDBError));
		}

		if(!pResult.found_entry) {
			return pState->SetError("No results found for %s", szIP);
		}

		MMDB_entry_data_s pEntryData;
		int iLookupStatus = MMDB_get_value(&pResult.entry, &pEntryData, "subdivisions", "0", "iso_code", NULL);

		if(iLookupStatus != MMDB_SUCCESS) {
			return pState->SetError("Failed to get data for %s (%s)", szIP, MMDB_strerror(iLookupStatus));
		}

		if(!pEntryData.has_data) {
			return pState->SetError("Failed to get data for %s (no data returned)", szIP);
		}
		
		if(pEntryData.type != MMDB_DATA_TYPE_UTF8_STRING) {
			return pState->SetError("Failed to parse data for %s", szIP);
		}
		
		SDK::StringValue Value(pEntryData.utf8_string, pEntryData.data_size);		
		State.Return(Value);
	
		MMDB_close(pMMDB);
		return true;

		SDK_ENDTRY;
	});	
	
	SDK::RegisterFunction("getTimeZoneName", [](Galactic3D::Interfaces::INativeState* pState, int32_t argc, void* pUser) {
		SDK_TRY;

		SDK::State State(pState);
		
		const char* szFileName = State.CheckString(0);

		MMDB_s* pMMDB = (MMDB_s*)malloc(sizeof(MMDB_s));
		if(pMMDB == NULL) {
			return pState->SetError("%s", MMDB_strerror(MMDB_OUT_OF_MEMORY_ERROR));
		}

		int iMMDBStatus = MMDB_open(szFileName, MMDB_MODE_MMAP, pMMDB);
		if(iMMDBStatus != MMDB_SUCCESS) {
			return pState->SetError("Could not load database file %s", szFileName);
		}

		const char* szIP = State.CheckString(1);

		int iGaiError = 0;
		int iMMDBError = MMDB_SUCCESS;
		MMDB_lookup_result_s pResult = MMDB_lookup_string(pMMDB, szIP, &iGaiError, &iMMDBError);

		if(iGaiError != 0) {
			return pState->SetError("%s does not appear to be an IPv4 or IPv6 address.", szIP);
		}

		if(iMMDBError != MMDB_SUCCESS) {
			return pState->SetError("Failed lookup for %s (%s)", szIP, MMDB_strerror(iMMDBError));
		}

		if(!pResult.found_entry) {
			return pState->SetError("No results found for %s", szIP);
		}

		MMDB_entry_data_s pEntryData;
		int iLookupStatus = MMDB_get_value(&pResult.entry, &pEntryData, "location", "time_zone", NULL);

		if(iLookupStatus != MMDB_SUCCESS) {
			return pState->SetError("Failed to get data for %s (%s)", szIP, MMDB_strerror(iLookupStatus));
		}

		if(!pEntryData.has_data) {
			return pState->SetError("Failed to get data for %s (no data returned)", szIP);
		}
		
		if(pEntryData.type != MMDB_DATA_TYPE_UTF8_STRING) {
			return pState->SetError("Failed to parse data for %s", szIP);
		}
		
		SDK::StringValue Value(pEntryData.utf8_string, pEntryData.data_size);		
		State.Return(Value);
	
		MMDB_close(pMMDB);
		return true;

		SDK_ENDTRY;
	});		
}

void ModuleUnregister()
{
}