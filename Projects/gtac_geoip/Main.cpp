#include <stdio.h>
#include <stdlib.h>
#include <SDKHelper.h>
#include <memory>

#include <maxminddb.h>

#ifndef _WIN32
#define sprintf_s snprintf
#endif

class CMMDB
{
public:
	inline CMMDB(MMDB_s* pMMDB) : m_pMMDB(pMMDB) {}
	inline ~CMMDB()
	{
		Close();
	}

	MMDB_s* m_pMMDB;

	void Close();
};

void CMMDB::Close()
{
	if (m_pMMDB != nullptr)
	{
		MMDB_close(m_pMMDB);
		m_pMMDB = nullptr;
	}
}

SDK::Class g_MMDBClass;

std::shared_ptr<SDK::Argument> FromEntryDataList(MMDB_entry_data_list_s **pEntryDataList);

// The modules internal name (Also used for the namespace name)
MODULE_MAIN("geoip");

void ModuleRegister()
{
	g_MMDBClass = SDK::Class("mmdb");
	
	SDK::RegisterFunction("init", [](Galactic3D::Interfaces::INativeState* pState, int32_t argc, void* pUser) {
		SDK_TRY;

		SDK::State State(pState);
		
		const char* szFileName = State.CheckString(0);
		char buff[100];
		
		MMDB_s* pMMDB = (MMDB_s*)malloc(sizeof(MMDB_s));
		if(pMMDB == NULL) {
			sprintf(buff, "[GeoIP] Error: %s", MMDB_strerror(MMDB_OUT_OF_MEMORY_ERROR));
			return pState->SetError(buff);
		}

		int status = MMDB_open(szFileName, MMDB_MODE_MMAP, pMMDB);
		if(status != MMDB_SUCCESS) {
			sprintf(buff, "[GeoIP] Error: Could not load database file %s", szFileName);
			return pState->SetError(buff);			
		}
		
		SDK::ClassValue<CMMDB, g_MMDBClass> Value(new CMMDB(pMMDB));	
		State.Return(Value);
		return true;

		SDK_ENDTRY;
	});
	
	g_MMDBClass.RegisterFunction("close", [](Galactic3D::Interfaces::INativeState* pState, int32_t argc, void* pUser) {
		SDK_TRY;

		SDK::State State(pState);

		auto pThis = State.CheckThis<CMMDB, g_MMDBClass>();
		MMDB_s* pMMDB = pThis->m_pMMDB;

		pThis->Close();

		return true;

		SDK_ENDTRY;
	});	

	g_MMDBClass.RegisterFunction("get", [](Galactic3D::Interfaces::INativeState* pState, int32_t argc, void* pUser) {
		SDK_TRY;

		SDK::State State(pState);

		auto pThis = State.CheckThis<CMMDB, g_MMDBClass>();
		MMDB_s* pMMDB = pThis->m_pMMDB;		
		
		if(pMMDB == NULL) {
			printf("[GeoIP] Handle is already closed!");
			return pState->SetError("[GeoIP] Handle is already closed!");
		}

		const char* szIP = State.CheckString(0);
		char buff[100];
		
		int iGaiError = 0;
		int iMMDBError = MMDB_SUCCESS;
		MMDB_lookup_result_s pLookupResult = MMDB_lookup_string(pMMDB, szIP, &iGaiError, &iMMDBError);

		if(iGaiError != 0) {
			sprintf(buff, "[GeoIP] '%s' does not appear to be an IPv4 or IPv6 address.", szIP);
			return pState->SetError(buff);
		}

		if(iMMDBError != MMDB_SUCCESS) {
			sprintf(buff, "[GeoIP] Error while looking up %s", szIP);
			return pState->SetError(buff);
		}

		if(!pLookupResult.found_entry) {
			sprintf(buff, "[GeoIP] No results found for %s", szIP);			
			return pState->SetError(buff);
		}
		
		MMDB_entry_data_list_s* pEntryDataList = NULL;
		int iStatus = MMDB_get_entry_data_list(&pLookupResult.entry, &pEntryDataList);
		if(iStatus != MMDB_SUCCESS) {
			sprintf(buff, "[GeoIP] Error while looking up data for %s", szIP);
			MMDB_free_entry_data_list(pEntryDataList);
			return pState->SetError(buff);			
		}

		MMDB_entry_data_list_s* pOriginalEntryDataList = pEntryDataList;
		
		printf("[GeoIP] Created pEntryDataList");
		
		//if(pEntryDataList == NULL || *pEntryDataList == NULL) {
		//	sprintf(buff, "[GeoIP] Error while looking up data. Your database may be corrupt.", szIP);
		//	return pState->SetError("[GeoIP] Error while looking up data. Your database may be corrupt.");			
		//}
		
		auto pResult = FromEntryDataList(&pEntryDataList);
		printf("[GeoIP] Used FromEntryDataList");
		
		MMDB_free_entry_data_list(pOriginalEntryDataList);
		printf("[GeoIP] Freed pOriginalEntryDataList");
		
		//auto NullValue = std::make_shared<SDK::NullValue>();
		//sprintf(buff, "[GeoIP] Set up null value");
		
		//if(pResult->m_pArgument == NULL) {
		//	return pState->SetError("[GeoIP] Error while looking up data. Your database may be corrupt.");			
		//}
		
		pState->Return(pResult->m_pArgument);
	
		return true;

		SDK_ENDTRY;
	});
}

void ModuleUnregister()
{
}

std::shared_ptr<SDK::Argument> FromEntryDataList(MMDB_entry_data_list_s **pEntryDataList)
{
	switch((*pEntryDataList)->entry_data.type) {
		case MMDB_DATA_TYPE_MAP: {
			auto AssocDictionary = std::make_shared<SDK::DictionaryValue>();
			
			const uint32_t iMapSize = (*pEntryDataList)->entry_data.data_size;
			for(uint32_t i = 0; i < iMapSize && pEntryDataList; ++i) {
				*pEntryDataList = (*pEntryDataList)->next;
				SDK::StringValue Value((*pEntryDataList)->entry_data.utf8_string);
				AssocDictionary->Set((*pEntryDataList)->entry_data.utf8_string, Value);

				*pEntryDataList = (*pEntryDataList)->next;
			}
			return AssocDictionary;
		}
		break;
		case MMDB_DATA_TYPE_ARRAY: {
			auto Array = std::make_shared<SDK::ArrayValue>();
			const uint32_t iSize = (*pEntryDataList)->entry_data.data_size;
			for (uint32_t i = 0; i < iSize && pEntryDataList; ++i) {
				*pEntryDataList = (*pEntryDataList)->next;
				SDK::StringValue Value((*pEntryDataList)->entry_data.utf8_string);
				Array->Insert(Value);
			}
			return Array;
		}
		break;
		case MMDB_DATA_TYPE_UTF8_STRING: {
			auto Value = std::make_shared<SDK::StringValue>((*pEntryDataList)->entry_data.utf8_string);
			return Value;
		}
		break;			
		//case MMDB_DATA_TYPE_BYTES: {
		//	SDK::StringValue Value((*pEntryDataList)->entry_data.bytes);
		//	return Value;
		//}
		break;			
		case MMDB_DATA_TYPE_DOUBLE: {
			auto Value = std::make_shared<SDK::NumberValue>((*pEntryDataList)->entry_data.double_value);
			return Value;
		}
		break;			
		case MMDB_DATA_TYPE_FLOAT: {
			auto Value = std::make_shared<SDK::NumberValue>((*pEntryDataList)->entry_data.float_value);
			return Value;
		}
		break;			
		case MMDB_DATA_TYPE_UINT16: {
			auto Value = std::make_shared<SDK::NumberValue>((*pEntryDataList)->entry_data.uint16);
			return Value;
		}
		break;			
		case MMDB_DATA_TYPE_UINT32: {
			auto Value = std::make_shared<SDK::NumberValue>((*pEntryDataList)->entry_data.uint32);
			return Value;
		}
		break;			
		case MMDB_DATA_TYPE_BOOLEAN: {
			auto Value = std::make_shared<SDK::BooleanValue>((*pEntryDataList)->entry_data.boolean);
			return Value;
		}
		break;			
		case MMDB_DATA_TYPE_UINT64: {
			auto Array = std::make_shared<SDK::ArrayValue>();
			uint32_t high = (*pEntryDataList)->entry_data.uint64 >> 32;
			uint32_t low = (uint32_t)(*pEntryDataList)->entry_data.uint64;

			SDK::NumberValue Value1(high);
			Array->Insert(Value1);

			SDK::NumberValue Value2(low);
			Array->Insert(Value2);			
			return Array;
		}
		break;			
		case MMDB_DATA_TYPE_UINT128: {
			auto Array = std::make_shared<SDK::ArrayValue>();
			uint64_t high = 0;
			uint64_t low = 0;
#if MMDB_UINT128_IS_BYTE_ARRAY
	int i;
	for (i = 0; i < 8; i++) {
		high = (high << 8) | (*pEntryDataList)->entry_data.uint128[i];
	}

	for (i = 8; i < 16; i++) {
		low = (low << 8) | (*pEntryDataList)->entry_data.uint128[i];
	}
#else
	high = (*pEntryDataList)->entry_data.uint128 >> 64;
	low = (uint64_t)(*pEntryDataList)->entry_data.uint128;
#endif

			uint32_t high_hi = high >> 32;
			uint32_t high_lo = (uint32_t)high;
			uint32_t low_hi = low >> 32;
			uint32_t low_lo = (uint32_t)low;
			
			SDK::NumberValue Value(high_hi);
			Array->Insert(Value);
			
			SDK::NumberValue Value2(high_lo);
			Array->Insert(Value2);
			
			SDK::NumberValue Value3(low_hi);
			Array->Insert(Value3);
			
			SDK::NumberValue Value4(low_lo);
			Array->Insert(Value4);
			
			return Array;
		}
		break;			
		case MMDB_DATA_TYPE_INT32: {	
			auto Value = std::make_shared<SDK::NumberValue>((*pEntryDataList)->entry_data.int32);
			return Value;
		}
		break;
	}
}

