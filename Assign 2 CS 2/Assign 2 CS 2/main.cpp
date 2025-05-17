#include "musicplaylist.h"
#include <iostream>
#include <string>
using namespace std;

int main() {
    // Step 1: Load the song library
    vector<Song*> library = loadSongLibrary("songs.csv");
    cout << "Loaded " << library.size() << " songs." << endl;

    // Step 2: Insert songs into hash table
    HashTable songTable;
    for (Song* song : library) {
        songTable.insert(song);
    }
    cout << "Added songs to hash table." << endl;

    // Step 3: Create play queue and history
    PlayQueue queue;
    PlayHistory history(5); // Keep last 5 songs in history

    // Step 4: Build initial playlist
    vector<string> tagsToLookup = { "Rock", "Pop" };
    vector<vector<Song*>> tagResults;

    cout << "\nBuilding playlist with tags: ";
    for (const auto& tag : tagsToLookup) {
        cout << tag << " ";
        tagResults.push_back(songTable.lookup(tag));
    }
    cout << endl;

    // Build initial playlist with breadth-first strategy
    buildBreadthFirstPlaylist(queue, tagResults, history);
    cout << "Playlist created with " << (!queue.isEmpty() ? "multiple" : "0") << " songs." << endl;

    // Step 5: Interactive command system
    cout << "\nAvailable commands:" << endl;
    cout << "- Play: Play the next song in queue" << endl;
    cout << "- Undo Last: Remove the most recent song from history" << endl;
    cout << "- Show History: Display recently played songs" << endl;
    cout << "- Peek Next: Show the next song in queue" << endl;
    cout << "- Exit: Quit the program" << endl;

    string command;
    while (true) {
        cout << "\n> ";
        getline(cin, command);

        if (command == "Play") {
            if (!queue.isEmpty()) {
                Song* currentSong = queue.dequeue();
                cout << "Now playing: " << currentSong->title << " by " << currentSong->artist << endl;
                history.addToHistory(currentSong);
            }
            else {
                cout << "No songs in queue to play." << endl;
            }
        }
        else if (command == "Undo Last") {
            Song* removed = history.removeLastPlayed();
            if (removed) {
                cout << "Removed " << removed->title << " by " << removed->artist << " from history." << endl;
            }
            else {
                cout << "No songs in history to remove." << endl;
            }
        }
        else if (command == "Show History") {
            cout << "Recently played songs:" << endl;
            history.printHistory();
        }
        else if (command == "Peek Next") {
            Song* next = queue.peek();
            if (next) {
                cout << "Next song: " << next->title << " by " << next->artist << endl;
            }
            else {
                cout << "No songs in queue." << endl;
            }
        }
        else if (command == "Exit") {
            break;
        }
        else {
            cout << "Unknown command. Available commands: Play, Undo Last, Show History, Peek Next, Exit" << endl;
        }
    }

    // Clean up
    for (Song* song : library) {
        delete song;
    }

    return 0;
}