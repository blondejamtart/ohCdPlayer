#include <iostream>
#include <sstream>
#include <cstdio>

#include "CdSource.h"

int main() {
    CdPlayer *player = new CdPlayer("/dev/sr0");
    std::cout << "Album \"" << player->AlbumName() << "\" by " << player->AlbumArtist() << "\nTracks:\n";
    int t = 0;
    for (CdTrack &track : player->iTracks) {
        std::cout << (++t) << ": \"" << track.Name() << "\" by " << track.Artist() << "\n";
    }
    std::cout << "Select Track to play: ";
    char input[100];
    std::cin >> input;
    uint16_t toPlay = atoi(input) - 1;
    
    void* buf = malloc(CDIO_CD_FRAMESIZE_RAW);
    if (toPlay < player->iTracks.size()) {
        CdTrack &track = player->iTracks[toPlay];
        int anim = 0;
        std::string bar = "----------";
        int wait = 0;
        FILE* alsa = popen("aplay -f cd", "w");
        while(track.GetAudio(buf) > 0) {
            if (wait == 20) {
                std::cout << "\rNow playing: \"" << track.Name() << "\" by " << track.Artist() << " ";
                bar[anim] = '~';
                std::cout << bar;
                bar[anim] = '-';
                anim = (anim + 1) % 10;
                wait = 0;
                std::cout.flush();
            }
            wait++;
            for (uint32_t i = 0; i < CDIO_CD_FRAMESIZE_RAW; i++) {
                fprintf(alsa, "%c", ((char *)buf)[i]);
            }
        }
        pclose(alsa);
    }
    free(buf);
    std::cout << "\r\n";
}