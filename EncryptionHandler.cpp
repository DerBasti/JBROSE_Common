#include <memory>
#include "EncryptionHandler.h"
#include "ROSEServer.h"

//-------------------------------------------------------------------------
//  This structure saves values while the tables are generated
//-------------------------------------------------------------------------
struct CryptTableControlStruct
{
	unsigned					CountTableMod[4];
	unsigned char				CurTableMod;
	unsigned					ValueTableMod[4];
};

//------------------------------------------------------------------------
// This structure is used by the encryption routines
//-------------------------------------------------------------------------
typedef unsigned long long int u64;

struct CryptBuffer
{
	u64		AddBufferLen : 11;
	u64		Command : 11;
	u64		AddTableValue : 11;
	u64		EncryptAddValue : 4;
	u64		EncryptValue : 3;
};

//-------------------------------------------------------------------------
// This table is used to calculate a checksum (size of 256 bytes)
//-------------------------------------------------------------------------
const unsigned char CheckSumTable[] =
"\x00\x5E\xBC\xE2\x61\x3F\xDD\x83\xC2\x9C\x7E\x20\xA3\xFD\x1F\x41"
"\x9D\xC3\x21\x7F\xFC\xA2\x40\x1E\x5F\x01\xE3\xBD\x3E\x60\x82\xDC"
"\x23\x7D\x9F\xC1\x42\x1C\xFE\xA0\xE1\xBF\x5D\x03\x80\xDE\x3C\x62"
"\xBE\xE0\x02\x5C\xDF\x81\x63\x3D\x7C\x22\xC0\x9E\x1D\x43\xA1\xFF"
"\x46\x18\xFA\xA4\x27\x79\x9B\xC5\x84\xDA\x38\x66\xE5\xBB\x59\x07"
"\xDB\x85\x67\x39\xBA\xE4\x06\x58\x19\x47\xA5\xFB\x78\x26\xC4\x9A"
"\x65\x3B\xD9\x87\x04\x5A\xB8\xE6\xA7\xF9\x1B\x45\xC6\x98\x7A\x24"
"\xF8\xA6\x44\x1A\x99\xC7\x25\x7B\x3A\x64\x86\xD8\x5B\x05\xE7\xB9"
"\x8C\xD2\x30\x6E\xED\xB3\x51\x0F\x4E\x10\xF2\xAC\x2F\x71\x93\xCD"
"\x11\x4F\xAD\xF3\x70\x2E\xCC\x92\xD3\x8D\x6F\x31\xB2\xEC\x0E\x50"
"\xAF\xF1\x13\x4D\xCE\x90\x72\x2C\x6D\x33\xD1\x8F\x0C\x52\xB0\xEE"
"\x32\x6C\x8E\xD0\x53\x0D\xEF\xB1\xF0\xAE\x4C\x12\x91\xCF\x2D\x73"
"\xCA\x94\x76\x28\xAB\xF5\x17\x49\x08\x56\xB4\xEA\x69\x37\xD5\x8B"
"\x57\x09\xEB\xB5\x36\x68\x8A\xD4\x95\xCB\x29\x77\xF4\xAA\x48\x16"
"\xE9\xB7\x55\x0B\x88\xD6\x34\x6A\x2B\x75\x97\xC9\x4A\x14\xF6\xA8"
"\x74\x2A\xC8\x96\x15\x4B\xA9\xF7\xB6\xE8\x0A\x54\xD7\x89\x6B\x35";

EncryptionHandler::EncryptionHandler()
{
	cryptStatus = new CryptStatus();
	memset(cryptStatus, 0x00, sizeof(CryptStatus));
}


EncryptionHandler::~EncryptionHandler()
{
	delete cryptStatus;
	cryptStatus = nullptr;
}

//----------------------------------------------------------------------------
// Private function, that generates the next value for the tables
//----------------------------------------------------------------------------
unsigned ctGenerateNextValue(CryptTableControlStruct *ControlStruct)
{
	// Get the last value depending on the current TableMod.
	int Value = ControlStruct->ValueTableMod[ControlStruct->CurTableMod];

	// Update generatecount, depending on the current TableMod
	ControlStruct->CountTableMod[ControlStruct->CurTableMod];

	// Calculate and return the next value
	switch (ControlStruct->CurTableMod)
	{
	case 0:
		Value = (int(Value * 0x8088405)) + 1;
		ControlStruct->ValueTableMod[0] = Value;
		return int(Value + (Value<0 ? 0xFFFF : 0)) >> 0x10;
	case 1:
		Value = (0x3039 - (int(Value * 0x3E39B193))) & 0x7FFFFFFF;
		ControlStruct->ValueTableMod[1] = Value;
		return Value;
	case 2:
		Value = (int(Value * 0x41C64E6D) + 0x3039);
		ControlStruct->ValueTableMod[2] = Value;
		return int(Value + (Value<0 ? 0xFFFF : 0)) >> 0x10;
	case 3:
		Value = (int(Value * 0x343FD) + 0x269EC3) & 0x7FFFFFFF;
		ControlStruct->ValueTableMod[3] = Value;
		return int(Value + (Value<0 ? 0xFFFF : 0)) >> 0x10;
	}

	return 0;
}

//-------------------------------------------------------------------------
// Private functions, that searches a value in an existing table to
// check if this value already exists.
//-------------------------------------------------------------------------
bool ctValueExistsInTable(CryptTable *CryptTables, unsigned char Table, unsigned Value, uint16_t EntryCount)
{
	for (uint16_t i = 0; i < EntryCount; ++i)
		if ((unsigned)CryptTables->getTables()[Table][i] == Value) return true;
	return false;
}

void CryptTable::generateCryptTables() {
	CryptTableControlStruct 	ControlStruct;
	unsigned char				TableFuncList[17];
	unsigned char				table;
	uint16_t				entry;
	unsigned					value;
	int							ivalue;
	unsigned ModValue = DEFAULT_CRYPTTABLE_START_VALUE;

	// Allocate memory for structure
	//tables = (unsigned  **)calloc(16, sizeof(unsigned));
	tables = new unsigned int*[0x10];

	// Prepare values
	ModValue = (int(ModValue * 0x41C64E6D) + 0x3039);
	ModValue = int(ModValue + ((int)ModValue<0 ? 0xFFFF : 0)) >> 0x10;

	// Startvalue for checksum variable for each user
	startValue = ModValue;

	// set startvalues
	for (table = 0; table<4; ++table)
	{
		ControlStruct.CountTableMod[table] = 0;
		ControlStruct.ValueTableMod[table] = ModValue;
	}

	// Generate TableMod list for creation
	ControlStruct.CurTableMod = 0;
	for (table = 0; table<0x11; ++table)
	{
		ivalue = ctGenerateNextValue(&ControlStruct) & 0x80000003;
		if (ivalue<0) ivalue = ((ivalue - 1) | 0xFFFFFFFC) + 1;
		TableFuncList[table] = (unsigned char)ivalue;
	}

	// Create tables
	for (table = 0; table<16; ++table)
	{
		// Prepare creation loop for current table
		//tables[table] = (unsigned*)calloc(0x800, sizeof(unsigned));
		tables[table] = new unsigned int[0x800];
		memset(tables[table], 0x00, 0x800 * sizeof(unsigned int));
		ControlStruct.CurTableMod = TableFuncList[table] & 3;

		// Generate current table
		entry = 0;
		do
		{
			// Generate next value
			value = ctGenerateNextValue(&ControlStruct);

			// Check, if value already exist in table 0
			if (ctValueExistsInTable(this, 0, value, entry)) continue;

			// Save value to memory
			tables[table][entry] = value;

			++entry;
		} while (entry<0x800);
	}

	// Generate additional table
	//additionalTables = (uint16_t*)calloc(0x200, sizeof(uint16_t));
	additionalTables = new uint16_t[0x200];
	memset(additionalTables, 0x00, 0x200 * sizeof(uint16_t));
	ControlStruct.CurTableMod = table = (unsigned char)(TableFuncList[0x10]) & 3;
	entry = 0;
	do
	{
		// Generate next value
		value = ctGenerateNextValue(&ControlStruct) & 0x7FF;

		// Check, if value already exist
		for (ivalue = 0; ivalue <= (int)entry; ++ivalue)
			if ((unsigned)additionalTables[ivalue] == value) { value = 0xFFFFFFFF; break; }

		// If value exist, continue
		if (value == 0xFFFFFFFF) continue;

		// Save value
		additionalTables[entry++] = value;
	} while (entry<0x200);
}


void LoginCryptTable::generateCryptTables()
{
	CryptTableControlStruct	ControlStruct;
	unsigned char				table;
	uint16_t				entry;
	unsigned					value;
	int							ivalue;

	// Allocate memory for structure
	tables = new unsigned int*[0x10];

	// Set default startvalues
	for (table = 0; table<4; ++table)
	{
		ControlStruct.CountTableMod[table] = 0;
		ControlStruct.ValueTableMod[table] = LOGIN_CRYPTTABLE_START_VALUE;
	}

	// Another startvalue
	startValue = LOGIN_CRYPTTABLE_START_VALUE;

	// Generate default tables
	for (table = 0; table<16; ++table)
	{
		// Prepare creation loop for current table
		ControlStruct.CurTableMod = table & 3;
		//tables[table] = (unsigned*)calloc(0x800, sizeof(unsigned));
		tables[table] = new unsigned int[0x800];
		memset(tables[table], 0x00, 0x800 * sizeof(unsigned int));

		entry = 0;
		do
		{
			// Generate next value depend on CurTableMod
			value = ctGenerateNextValue(&ControlStruct);

			// Only take value, if it doesn't exist in table 0 already
			if (ctValueExistsInTable(this, 0, value, entry)) continue;

			// Save value to structure
			tables[table][entry] = value;

			++entry;
		} while (entry<0x800);
	}

	// Create additional table
	//additionalTables = (uint16_t*)calloc(0x200, sizeof(uint16_t));
	additionalTables = new uint16_t[0x200];
	memset(additionalTables, 0x00, 0x200 * sizeof(uint16_t));
	entry = 0;
	do
	{
		// Generate next value for table
		ControlStruct.CountTableMod[1];
		ivalue = ControlStruct.ValueTableMod[1];
		ivalue *= 0x3E39B193;
		value = (0x3039 - (unsigned)ivalue) & 0x7FFFFFFF;
		ControlStruct.ValueTableMod[1] = value;

		// Don't save, if value already exist
		value &= 0x7FF;
		for (ivalue = 0; ivalue <= (int)entry; ++ivalue)
			if ((unsigned)additionalTables[ivalue] == value) { value = 0xFFFFFFFF; break; }

		// If value already exist, continue
		if (value == 0xFFFFFFFF) continue;

		// Save value to structure
		additionalTables[entry] = value;

		++entry;
	} while (entry<0x200);
}

void CryptTable::freeCryptTables()
{
	unsigned char c;

	// Free every table
	for (c = 0; c < 16; ++c) {
		//free(tables[c]);
		delete[] tables[c];
	}

	// Free memory
	//free(additionalTables);
	//free(tables);

	delete[] additionalTables;
	delete[] tables;
}

uint16_t EncryptionHandler::decryptHeader(unsigned char *Buffer) {
	CryptBuffer		TempBuffer;
	unsigned		BigBuffer;
	unsigned char		c;
	const CryptTable *globalTable = ROSEServer::getEncryptionTable();

	// Read "AddTableValue" from buffer
	BigBuffer = *((unsigned*)Buffer);
	TempBuffer.AddTableValue = ((Buffer[4] >> 0x03) & 0x0007) | ((BigBuffer) & 0x0038) | \
		((BigBuffer >> 0x03) & 0x01C0) | ((BigBuffer >> 0x11) & 0x0600);

	// Decrypt header in the buffer
	for (c = 0; c<5; ++c)
		Buffer[c] ^= globalTable->getTables()[c][TempBuffer.AddTableValue];

	BigBuffer = *((unsigned*)Buffer);
	TempBuffer.EncryptValue = (BigBuffer >> 0x0F) & 0x0007;
	TempBuffer.Command = ((BigBuffer >> 0x0C) & 0x0007) | ((BigBuffer << 0x03) & 0x0038) | \
		((Buffer[4] << 0x06) & 0x01C0) | ((BigBuffer >> 0x13) & 0x0600);
	TempBuffer.AddBufferLen = ((BigBuffer >> 0x06) & 0x0007) | ((BigBuffer >> 0x0F) & 0x0038) | \
		((BigBuffer >> 0x11) & 0x01C0) | ((Buffer[4] << 0x03) & 0x0600);
	TempBuffer.EncryptAddValue = ((BigBuffer >> 0x1E) & 0x0003) | ((BigBuffer >> 0x13) & 0x000C);

	// Size correct?
	if (TempBuffer.AddBufferLen < 0x06 || TempBuffer.AddBufferLen > 0x400)
		return 0;

	// Header Checksum correct? ~ not implented yet ~
	/*
	if ( ((unsigned)( TempBuffer.AddTableValue + TempBuffer.EncryptAddValue ) & 7 ) != \
	((unsigned)( TempBuffer.EncryptValue                               ) & 7 ) )
	return 0;
	*/

	// Save CryptBuffer temporary to buffer for later use
	*((unsigned*)Buffer) = *((unsigned     *)&TempBuffer);
	Buffer[4] = ((unsigned char*)&TempBuffer)[4];

	// Update encryption value
	cryptStatus->currentValue = (cryptStatus->currentValue * 0x343FD) + 0x269EC3;

	// Everything went ok
	return static_cast<uint16_t>(TempBuffer.AddBufferLen);
}

bool EncryptionHandler::decryptBuffer(unsigned char *Buffer) {
	CryptBuffer		TempBuffer;
	uint16_t		c, BufferLen;
	unsigned char		CheckSum;

	// Read CryptBuffer from received buffer
	*((unsigned     *)&TempBuffer) = *((unsigned*)Buffer);
	((unsigned char*)&TempBuffer)[4] = Buffer[4];

	// Calculate CheckSum for bufferheader
	for (CheckSum = 0, c = 0; c<5; ++c)
		CheckSum = CheckSumTable[(unsigned char)Buffer[c] ^ CheckSum];

	// Calculate real buffer length and check it
	BufferLen = (uint16_t)(TempBuffer.AddBufferLen - TempBuffer.EncryptValue);
	if (BufferLen < 0x06 || BufferLen > 0x400) return false;

	// Decrypt buffer data
	if (BufferLen > 0x06)
	{
		unsigned int **tables = ROSEServer::getEncryptionTable()->getTables();
		for (c = 6; c<BufferLen; ++c)
		{
			Buffer[c] ^= tables[(TempBuffer.EncryptAddValue + c) & 0x000F] \
				[(TempBuffer.AddTableValue + c) & 0x07FF];
			CheckSum = CheckSumTable[(unsigned char)Buffer[c] ^ CheckSum];
		}
	}

	// Check CheckSum
	if (Buffer[5] != CheckSum)
		return false;

	// Complete buffer
	*((uint16_t*)(Buffer)) = BufferLen;
	*((uint16_t*)(Buffer + 2)) = TempBuffer.Command;

	return true;
}

void EncryptionHandler::encryptBuffer(unsigned char *Buffer) {
	CryptBuffer		TempBuffer;
	uint16_t		c;
	unsigned char		CheckSum;
	unsigned			EncryptValue;
	unsigned int **cryptTables = ROSEServer::getEncryptionTable()->getTables();

	// Generate some random values for encrypting
	TempBuffer.AddTableValue = (rand() % 0x800);
	TempBuffer.EncryptAddValue = (rand() % 0x10);
	TempBuffer.EncryptValue = TempBuffer.AddTableValue + TempBuffer.EncryptAddValue;

	// Fill other fields with values from Buffer
	TempBuffer.AddBufferLen = *((uint16_t*)(Buffer));
	TempBuffer.Command = *((uint16_t*)(Buffer + 2));

	// Build new bufferheader	

	Buffer[4] = (((unsigned)TempBuffer.AddBufferLen) & 0x00000007) ^ \
		(((unsigned)TempBuffer.Command >> 0x03) & 0x000000C0);
	EncryptValue = (((unsigned)TempBuffer.AddBufferLen >> 0x03) & 0x00000007);
	EncryptValue |= (((unsigned)TempBuffer.AddBufferLen << 0x06) & 0x00007000);
	EncryptValue |= (((unsigned)TempBuffer.AddBufferLen << 0x13) & 0x30000000);
	EncryptValue |= (((unsigned)TempBuffer.Command << 0x17) & 0x03800000);
	EncryptValue |= (((unsigned)TempBuffer.Command << 0x0F) & 0x001C0000);
	EncryptValue |= (((unsigned)TempBuffer.Command) & 0x000001C0);
	EncryptValue |= (((unsigned)TempBuffer.EncryptValue << 0x09) & 0x00000E00);
	EncryptValue |= (((unsigned)TempBuffer.EncryptAddValue << 0x1A) & 0x0C000000);
	EncryptValue |= (((unsigned)TempBuffer.EncryptAddValue << 0x1C) & 0xC0000000);

	*((unsigned*)Buffer) = EncryptValue;

	// Encrypt bufferheader
	for (CheckSum = 0, c = 0; c<5; ++c)
	{
		CheckSum = CheckSumTable[((unsigned char*)&TempBuffer)[c] ^ CheckSum];
		Buffer[c] ^= cryptTables[c][TempBuffer.AddTableValue];
	}


	// Encrypt bufferdata if exists
	if (TempBuffer.AddBufferLen > 0x06)
	{
		for (c = 6; c<TempBuffer.AddBufferLen; c++)
		{
			CheckSum = CheckSumTable[(unsigned char)Buffer[c] ^ CheckSum];
			unsigned char cryptTableValue = cryptTables[(TempBuffer.EncryptAddValue + c) & 0x000F][(TempBuffer.AddTableValue + c) & 0x07FF];
			Buffer[c] ^= cryptTableValue;
		}
	}
	Buffer[5] = CheckSum;

	// Complete bufferheader
	EncryptValue = *((unsigned*)Buffer) & 0xFF9C7FC7;
	EncryptValue |= (((unsigned)TempBuffer.AddTableValue << 0x03) & 0x00000038);
	EncryptValue |= (((unsigned)TempBuffer.AddTableValue << 0x09) & 0x00038000);
	EncryptValue |= (((unsigned)TempBuffer.AddTableValue << 0x0C) & 0x00600000);
	Buffer[4] = (Buffer[4] & 0xC7) | (((unsigned)TempBuffer.AddTableValue) & 0x38);
	*((unsigned*)Buffer) = EncryptValue;
}