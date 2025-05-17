#ifndef MUSIC_PLAYLIST_H
#define MUSIC_PLAYLIST_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
using namespace std;

// Song structure definition
struct Song {
    int id;                // Unique identifier
    string title;     // Song title
    string artist;    // Artist name
    string genre;     // Genre
    string mood;      // Mood tag

    Song(int id, string t, string a, string g, string m)
        : id(id), title(t), artist(a), genre(g), mood(m) {}
};

// Forward declaration of PlayHistory for use in playlist generation
class PlayHistory;

// Node for hash table chaining
struct Node {
    Song* song;
    Node* next;

    Node(Song* s) : song(s), next(nullptr) {}
};

// Hash table class declaration
class HashTable {
private:
    static const int TABLE_SIZE = 101; // Prime number for better distribution
    Node* table[TABLE_SIZE];

    // Hash function as a member function
    unsigned long hashFunction(const string& key);

public:
    HashTable();
    void insert(Song* song);
    void remove(Song* song);
    vector<Song*> lookup(const string& tagValue);
    ~HashTable();
};

// Play queue class declaration
class PlayQueue {
private:
    struct QNode {
        Song* song;
        QNode* next;
        QNode(Song* s) : song(s), next(nullptr) {}
    };

    QNode* head; // Front of queue
    QNode* tail; // Back of queue

public:
    PlayQueue();
    void enqueue(Song* song);
    Song* dequeue();
    Song* peek();
    bool isEmpty();
    ~PlayQueue();
};

// Play history class declaration
class PlayHistory { 
private:
    struct HistoryNode {
        Song* song;
        int playCount;
        HistoryNode* next;

        HistoryNode(Song* s) : song(s), playCount(1), next(nullptr) {}
    };

    HistoryNode* head; // Oldest entry
    int capacity;      // Maximum number of entries
    int count;         // Current count of entries

public:
    PlayHistory(int cap = 10);
    void addToHistory(Song* song);
    void printHistory();
    bool wasRecentlyPlayed(Song* song, int lookback = 3);
    Song* removeLastPlayed();
    ~PlayHistory();
};

// Playlist generation function prototypes
void buildBreadthFirstPlaylist(PlayQueue& queue,
    const vector<vector<Song*>>& tagResults, PlayHistory& history);

void buildWeightedShufflePlaylist(PlayQueue& queue,
    const vector<vector<Song*>>& tagResults, PlayHistory& history);

// File I/O function prototypes
vector<Song*> loadSongLibrary(const string& filename);
void savePlaylist(PlayQueue& queue, const string& filename);

#endif