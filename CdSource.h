#include <cdio/cdtext.h>
#include <cdio/mmc.h>

#include <cstdint>
#include <string>
#include <vector>

class CdPlayer;

class CdTrack {
public: 
    CdTrack(CdPlayer &aParent, uint16_t aIndex, const char* aName, const char* aArtist, const char* aGenre, uint32_t aFirstSector, uint32_t aLastSector);
    std::string Name();
    std::string Artist();
    std::string Genre();
    int64_t GetAudio(void* aBuffer);
private:
    uint16_t iIndex;
    std::string iName;
    std::string iArtist;
    std::string iGenre;
    uint32_t iFirstSector;
    uint32_t iLastSector;
    uint32_t iCurrentSector;
    CdPlayer &iParent;
};

class CdPlayer {
friend CdTrack;
public:
    CdPlayer(const char* aDeviceName);
    ~CdPlayer();

    std::vector<CdTrack> iTracks;

    std::string AlbumName();
    std::string AlbumArtist();

private:
    char* iDeviceName;
    std::string iAlbumArtist;
    std::string iAlbumName;

    CdIo_t                *iCdioPtr;
    cdio_fs_anal_t         iCdFsType;

    size_t ReadSector(void*aBuffer, uint32_t aSector);
};