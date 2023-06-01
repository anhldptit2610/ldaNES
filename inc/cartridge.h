
enum MIRRORING {
	HORIZONTAL,
	VERTICAL,
	FOUR_SCREEN,
};

enum CARTRIDGE_VERSION {
	iNES,
	NESv2,
};

struct cartridge {
	uint8_t header[16];
	uint8_t prgromSize;
	uint8_t chrromSize;
	uint8_t prgramSize;
	enum MIRRORING mirroringMode;
	enum CARTRIDGE_VERSION cartridgeVersion;
	uint8_t mapperID;
	bool skipTrainer;
	bool isCartridgeUsesPRGRAM;
};

void parseCartridge(struct cartridge *cart, struct bus *bus, char *rom_path);
