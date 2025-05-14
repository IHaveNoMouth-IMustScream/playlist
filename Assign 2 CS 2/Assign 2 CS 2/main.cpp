#include "musicplaylist.h"
#include <iostream>
#include <string>

int main() {
    // Step 1: Load the song library
    std::vector<Song*> library = loadSongLibrary("songs.csv");
    std::cout << "Loaded " << library.size() << " songs." << std::endl;

    // Step 2: Insert songs into hash table
    HashTable songTable;
    for (Song* song : library) {
        songTable.insert(song);
    }
    std::cout << "Added songs to hash table." << std::endl;

    // Step 3: Create play queue and history
    PlayQueue queue;
    PlayHistory history(5); // Keep last 5 songs in history

    // Step 4: Build initial playlist
    std::vector<std::string> tagsToLookup = { "Rock", "Pop" };
    std::vector<std::vector<Song*>> tagResults;

    std::cout << "\nBuilding playlist with tags: ";
    for (const auto& tag : tagsToLookup) {
        std::cout << tag << " ";
        tagResults.push_back(songTable.lookup(tag));
    }
    std::cout << std::endl;

    // Build initial playlist with breadth-first strategy
    buildBreadthFirstPlaylist(queue, tagResults, history);
    std::cout << "Playlist created with " << (!queue.isEmpty() ? "multiple" : "0") << " songs." << std::endl;

    // Step 5: Interactive command system
    std::cout << "\nAvailable commands:" << std::endl;
    std::cout << "- Play: Play the next song in queue" << std::endl;
    std::cout << "- Undo Last: Remove the most recent song from history" << std::endl;
    std::cout << "- Show History: Display recently played songs" << std::endl;
    std::cout << "- Peek Next: Show the next song in queue" << std::endl;
    std::cout << "- Exit: Quit the program" << std::endl;

    std::string command;
    while (true) {
        std::cout << "\n> ";
        std::getline(std::cin, command);

        if (command == "Play") {
            if (!queue.isEmpty()) {
                Song* currentSong = queue.dequeue();
                std::cout << "Now playing: " << currentSong->title << " by " << currentSong->artist << std::endl;
                history.addToHistory(currentSong);
            }
            else {
                std::cout << "No songs in queue to play." << std::endl;
            }
        }
        else if (command == "Undo Last") {
            Song* removed = history.removeLastPlayed();
            if (removed) {
                std::cout << "Removed " << removed->title << " by " << removed->artist << " from history." << std::endl;
            }
            else {
                std::cout << "No songs in history to remove." << std::endl;
            }
        }
        else if (command == "Show History") {
            std::cout << "Recently played songs:" << std::endl;
            history.printHistory();
        }
        else if (command == "Peek Next") {
            Song* next = queue.peek();
            if (next) {
                std::cout << "Next song: " << next->title << " by " << next->artist << std::endl;
            }
            else {
                std::cout << "No songs in queue." << std::endl;
            }
        }
        else if (command == "Exit") {
            break;
        }
        else {
            std::cout << "Unknown command. Available commands: Play, Undo Last, Show History, Peek Next, Exit" << std::endl;
        }
    }

    // Clean up
    for (Song* song : library) {
        delete song;
    }

    return 0;
}