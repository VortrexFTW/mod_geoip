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

static SDK::Argument FromEntryDataList(MMDB_entry_data_list_s **pEntryDataList);

// The modules internal name (Also used for the namespace name)
MODULE_MAIN("geoip");

void ModuleRegister()
{
	SDK::RegisterFunction("get", [](Galactic3D::Interfaces::INativeState* pState, int32_t argc, void* pUser) {
		SDK_TRY;

		SDK::State State(pState);
		
		const char* szFileName = State.CheckString(0);

		MMDB_s* pMMDB = (MMDB_s*)malloc(sizeof(MMDB_s));
		if(pMMDB == NULL) {
			return pState->SetError("[GeoIP] Error: %s", MMDB_strerror(MMDB_OUT_OF_MEMORY_ERROR));
		}

		int status = MMDB_open(szFileName, MMDB_MODE_MMAP, pMMDB);
		if(status != MMDB_SUCCESS) {
			return pState->SetError("[GeoIP] Error: Could not load database file %s", szFileName);
		}

		if(pMMDB == NULL) {
			return pState->SetError("[GeoIP] Handle is already closed!");
		}
		
		printf("MMDB object valid\n");

		const char* szIP = State.CheckString(1);

		int iGaiError = 0;
		int iMMDBError = MMDB_SUCCESS;
		MMDB_lookup_result_s pLookupResult = MMDB_lookup_string(pMMDB, szIP, &iGaiError, &iMMDBError);
		printf("Looked up string\n");

		if(iGaiError != 0) {
			return pState->SetError("[GeoIP] '%s' does not appear to be an IPv4 or IPv6 address.", szIP);
		}
		printf("IP Valid\n");

		if(iMMDBError != MMDB_SUCCESS) {
			return pState->SetError("[GeoIP] Error while looking up %s", szIP);
		}
		printf("MMDB opened\n");

		if(!pLookupResult.found_entry) {
			return pState->SetError("[GeoIP] No results found for %s", szIP);
		}
		printf("Results found\n");

		MMDB_entry_data_list_s* pEntryDataList = NULL;
		int iStatus = MMDB_get_entry_data_list(&pLookupResult.entry, &pEntryDataList);
		if(iStatus != MMDB_SUCCESS) {
			MMDB_free_entry_data_list(pEntryDataList);
			return pState->SetError("[GeoIP] Error while looking up data for %s", szIP);
		}
		printf("Entry data list retrieved\n");

		if(!pEntryDataList) {
			return pState->SetError("[GeoIP] Error while looking up data. Your database may be corrupt.", szIP);
		}
		printf("Entry data list valid\n");
			
		auto pOriginalEntryDataList = pEntryDataList;

		SDK::Argument pResult = FromEntryDataList(&pEntryDataList);
		printf("Result acquired from entry data list\n");
		
		MMDB_free_entry_data_list(pOriginalEntryDataList);
		printf("Free'd original entry data list\n");
		
		MMDB_free_entry_data_list(pEntryDataList);
		printf("Free'd entry data list\n");		

		printf("Checking result argument\n");
		SDK::NullValue NullVal;
		if(pResult == NullVal) {
			return pState->SetError("[GeoIP] Error while looking up data. Your database may be corrupt.");
		}
		printf("Result argument valid\n");
		
		printf("Closing MMDB stuff\n");
		MMDB_close(pMMDB);
		free(pMMDB);
		
		printf("Returning result\n");

		//printf("Country name is %s", pResult.Get("country").Get("names").Get("en"));
		//State.Return(pResult);

		return true;

		SDK_ENDTRY;
	});
}

void ModuleUnregister()
{
}

static SDK::Argument FromEntryDataList(MMDB_entry_data_list_s **pEntryDataList)
{
	switch((*pEntryDataList)->entry_data.type) {
		case MMDB_DATA_TYPE_MAP: {
			printf("Data type is map\n");
			SDK::DictionaryValue AssocDictionary;

			const uint32_t iMapSize = (*pEntryDataList)->entry_data.data_size;
			printf("Map size is %d\n", iMapSize);
			for(uint32_t i = 0; i < iMapSize && pEntryDataList; ++i) {	
				printf("Map item iter %d\n", i);

				auto ppEntryDataList = pEntryDataList;
				auto pResult = FromEntryDataList(ppEntryDataList);
				
				if(!pResult) {
					continue;
				}
				
				printf("Setting dictionary item '%s' for iter %d\n", (*pEntryDataList)->entry_data.utf8_string, i);
				AssocDictionary.Set((*pEntryDataList)->entry_data.utf8_string, pResult);
				
				printf("Freeing temp entry data list for iter %d\n", i);
				MMDB_free_entry_data_list(*ppEntryDataList);

				*pEntryDataList = (*pEntryDataList)->next;
				printf("Next method used for iter %d\n", i);
			}
			printf("Completed loop\n");
			return AssocDictionary;
		}
		break;
		case MMDB_DATA_TYPE_ARRAY: {
			SDK::ArrayValue Array;

			const uint32_t iMapSize = (*pEntryDataList)->entry_data.data_size;
			for(uint32_t i = 0; i < iMapSize && pEntryDataList; ++i) {		
				printf("Array item iter %d\n", i);
				
				auto ppEntryDataList = pEntryDataList;
				auto pResult = FromEntryDataList(ppEntryDataList);
				
				if(!pResult) {
					continue;
				}
				
				printf("Inserting array item for iter %d\n", i);
				Array.Insert(pResult);
				
				printf("Freeing temp entry data list for iter %d\n", i);
				MMDB_free_entry_data_list(*ppEntryDataList);				

				*pEntryDataList = (*pEntryDataList)->next;
				printf("Next method used for iter %d\n", i);
			}
			printf("Completed loop. Returning array\n");
			return Array;
		}
		break;
		case MMDB_DATA_TYPE_UTF8_STRING: {
			printf("Data type is string (value: %s)\n", (*pEntryDataList)->entry_data.utf8_string);
			SDK::StringValue Value((*pEntryDataList)->entry_data.utf8_string);
			return Value;
		}
		break;
		case MMDB_DATA_TYPE_DOUBLE: {
			printf("Data type is string (value: %f)\n", (*pEntryDataList)->entry_data.double_value);
			SDK::NumberValue Value((*pEntryDataList)->entry_data.double_value);
			return Value;
		}
		break;
		case MMDB_DATA_TYPE_FLOAT: {
			printf("Data type is float (value: %f)\n", (*pEntryDataList)->entry_data.float_value);
			SDK::NumberValue Value((*pEntryDataList)->entry_data.float_value);
			return Value;
		}
		break;
		case MMDB_DATA_TYPE_UINT16: {
			printf("Data type is uint16\n");
			SDK::NumberValue Value((*pEntryDataList)->entry_data.uint16);
			return Value;
		}
		break;
		case MMDB_DATA_TYPE_UINT32: {
			printf("Data type is uint32\n");
			SDK::NumberValue Value((*pEntryDataList)->entry_data.uint32);
			return Value;
		}
		break;
		case MMDB_DATA_TYPE_BOOLEAN: {
			printf("Data type is bool\n");
			SDK::BooleanValue Value((*pEntryDataList)->entry_data.boolean);
			return Value;
		}
		break;
		case MMDB_DATA_TYPE_UINT64: {
			printf("Data type is uint64\n");
			SDK::ArrayValue Array;
			uint32_t high = (*pEntryDataList)->entry_data.uint64 >> 32;
			uint32_t low = (uint32_t)(*pEntryDataList)->entry_data.uint64;

			SDK::NumberValue Value1(high);
			Array.Insert(Value1);

			SDK::NumberValue Value2(low);
			Array.Insert(Value2);
			return Array;
		}
		break;
		case MMDB_DATA_TYPE_UINT128: {
			printf("Data type is uint128\n");
			SDK::ArrayValue Array;
			uint64_t high = 0;
			uint64_t low = 0;
			
			high = (*pEntryDataList)->entry_data.uint128 >> 64;
			low = (uint64_t)(*pEntryDataList)->entry_data.uint128;

			uint32_t high_hi = high >> 32;
			uint32_t high_lo = (uint32_t)high;
			uint32_t low_hi = low >> 32;
			uint32_t low_lo = (uint32_t)low;

			SDK::NumberValue Value1(high_hi);
			Array.Insert(Value1);

			SDK::NumberValue Value2(high_lo);
			Array.Insert(Value2);

			SDK::NumberValue Value3(low_hi);
			Array.Insert(Value3);

			SDK::NumberValue Value4(low_lo);
			Array.Insert(Value4);

			return Array;
		}
		break;
		case MMDB_DATA_TYPE_INT32: {
			printf("Data type is int32\n");
			SDK::NumberValue Value((*pEntryDataList)->entry_data.int32);
			return Value;
		}
		break;
	}
}

