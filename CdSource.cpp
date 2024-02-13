#include <exception>
#include <cstdio>

#include <cdio/util.h>

#include "CdSource.h"

const cdtext_lang_t kDefaultLanguage = CDTEXT_LANGUAGE_ENGLISH;

CdPlayer::CdPlayer(const char* aDeviceName) {
    iCdioPtr = cdio_open_am(aDeviceName, DRIVER_DEVICE, NULL);
    cdtext_t *cdTextPtr = cdio_get_cdtext(iCdioPtr);
    uint32_t firstTrack = cdio_get_first_track_num(iCdioPtr);
    uint32_t numTracks = cdio_get_num_tracks(iCdioPtr);
    uint32_t startSector = cdio_get_track_lsn(iCdioPtr, firstTrack);  

    cdtext_lang_t *infoLanguages = cdtext_list_languages_v2(cdTextPtr);
    for (uint16_t i = 0; i < 8; i++) {
        if (infoLanguages[i] == kDefaultLanguage) {
            cdtext_set_language_index(cdTextPtr, i);
            break;
        }
    }

    iAlbumName = cdtext_get_const(cdTextPtr, CDTEXT_FIELD_TITLE, 0);
    iAlbumArtist = cdtext_get_const(cdTextPtr, CDTEXT_FIELD_PERFORMER, 0);    

    uint32_t previousTrackLastSector = 0; 
    for (uint32_t i = firstTrack; i < firstTrack + numTracks; i++) {
        msf_t trackMsf;
        if (!cdio_get_track_msf(iCdioPtr, i, &trackMsf)) {
            // throw(std::exception());
        }
        track_format_t trackFormat = cdio_get_track_format(iCdioPtr, i);
        uint32_t thisTrackLastSector = cdio_msf_to_lsn(&trackMsf);
        if (trackFormat == TRACK_FORMAT_AUDIO && thisTrackLastSector != 0) {
            iTracks.emplace_back(CdTrack(
                *this,
                i,
                cdtext_get_const(cdTextPtr, CDTEXT_FIELD_TITLE, i-1),
                cdtext_get_const(cdTextPtr, CDTEXT_FIELD_PERFORMER, i-1),
                "", previousTrackLastSector, thisTrackLastSector)
            );
        }
        previousTrackLastSector = thisTrackLastSector;
    }
   
}

std::string CdPlayer::AlbumName() {
    return std::string(iAlbumName);
}

std::string CdPlayer::AlbumArtist() {
    return std::string(iAlbumArtist);
}

size_t CdPlayer::ReadSector(void* aBuffer, uint32_t aSector) {
    cdio_read_sector(iCdioPtr, aBuffer, aSector, CDIO_READ_MODE_AUDIO);
    return 1;
}

CdTrack::CdTrack(CdPlayer &aParent, uint16_t aIndex, const char* aName, const char* aArtist, const char* aGenre, uint32_t aFirstSector, uint32_t aLastSector) : 
    iParent(aParent),
    iIndex(aIndex),
    iFirstSector(aFirstSector),
    iLastSector(aLastSector),
    iCurrentSector(aFirstSector)
{
    if (!aName) {
        char name[10];
        sprintf(name, "Track %02d", iIndex);
        iName = name;
    } else {
        iName = aName;
    }
    if (!aArtist) {
        iArtist = "Unknown Artist";
    } else {
        iArtist = aArtist;
    }
    if (!aGenre) {
        iGenre = "Unknown Genre";
    } else {
        iGenre = aGenre;
    }
}

int64_t CdTrack::GetAudio(void* aBuffer) {
    if (iCurrentSector == iLastSector) {
        iCurrentSector = iFirstSector;
        return -1;
    }
    size_t ret = iParent.ReadSector(aBuffer, iCurrentSector);
    //std::cout << "Read sector " << iCurrentSector << "\n";
    iCurrentSector++;
    return ret;
}

std::string CdTrack::Name() {
    return std::string(iName);
}

std::string CdTrack::Artist() {
    return std::string(iArtist);
}

std::string CdTrack::Genre() {
    return std::string(iGenre);
}