#pragma once

class CryptTable {
protected:
	unsigned int** tables;
	uint16_t* additionalTables;

	unsigned int startValue;

	virtual void freeCryptTables();
public:
	const static uint32_t LOGIN_CRYPTTABLE_START_VALUE = 0x0042D266;
	const static uint32_t DEFAULT_CRYPTTABLE_START_VALUE = 0x87654321;
	CryptTable() {
	}
	virtual ~CryptTable() {
		freeCryptTables();
	}
	virtual void generateCryptTables();

	unsigned int **getTables() const {
		return tables;
	}
	uint16_t *getAdditionalTables() const {
		return additionalTables;
	}
	unsigned int getStartValue() const {
		return startValue;
	}
};

class LoginCryptTable : public CryptTable {
public:
	LoginCryptTable() {
	}
	virtual ~LoginCryptTable() {
	}
	virtual void generateCryptTables();
};

struct CryptStatus {
	unsigned int currentValue;
	unsigned int additionalValuePerStep;
};


class EncryptionHandler
{
private:
	CryptStatus *cryptStatus;
public:
	EncryptionHandler();
	~EncryptionHandler();

	__inline void setEncryptionValue(unsigned long encryptionValue) {
		cryptStatus->additionalValuePerStep = encryptionValue;
	}

	uint16_t decryptHeader(unsigned char *Buffer);
	bool decryptBuffer(unsigned char *Buffer);
	void encryptBuffer(unsigned char *Buffer);
};