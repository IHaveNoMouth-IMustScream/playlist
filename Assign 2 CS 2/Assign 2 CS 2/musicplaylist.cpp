#include "musicplaylist.h"


unsigned long HashTable::hashFunction(const std::string& key) {
    unsigned long hash = 0;
    for (char c : key) {
        hash = hash * 31 + c;  // polynomial rolling hash
    }
    return hash % TABLE_SIZE;
}

HashTable::HashTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        table[i] = nullptr;
    }
}

void HashTable::insert(Song* song) {
    // Using genre as the key for this example
    unsigned long index = hashFunction(song->genre);
    Node* newNode = new Node(song);
    newNode->next = table[index];
    table[index] = newNode;
}

void HashTable::remove(Song* song) {
    unsigned long index = hashFunction(song->genre);
    Node* current = table[index];
    Node* prev = nullptr;
    while (current) {
        if (current->song->id == song->id) {
            if (prev) {
                prev->next = current->next;
            }
            else {
                table[index] = current->next;
            }
            delete current;
            return;
        }
        prev = current;
        current = current->next;
    }
}

std::vector<Song*> HashTable::lookup(const std::string& tagValue) {
    std::vector<Song*> results;
    unsigned long index = hashFunction(tagValue);
    Node* current = table[index];
    while (current) {
        if (current->song->genre == tagValue) {
            results.push_back(current->song);
        }
        current = current->next;
    }
    return results;
}

HashTable::~HashTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node* current = table[i];
        while (current) {
            Node* toDelete = current;
            current = current->next;
            delete toDelete;
        }
    }
}

// PlayQueue implementation

PlayQueue::PlayQueue() : head(nullptr), tail(nullptr) {}

void PlayQueue::enqueue(Song* song) {
    QNode* newNode = new QNode(song);
    if (!tail) {
        head = tail = newNode;
    }
    else {
        tail->next = newNode;
        tail = newNode;
    }
}

Song* PlayQueue::dequeue() {
    if (!head) return nullptr;
    QNode* temp = head;
    Song* song = head->song;
    head = head->next;
    if (!head) tail = nullptr;
    delete temp;
    return song;
}

Song* PlayQueue::peek() {
    return head ? head->song : nullptr;
}

bool PlayQueue::isEmpty() {
    return head == nullptr;
}

PlayQueue::~PlayQueue() {
    while (head) {
        QNode* temp = head;
        head = head->next;
        delete temp;
    }
}

// PlayHistory implementation

PlayHistory::PlayHistory(int cap) : head(nullptr), capacity(cap), count(0) {}

void PlayHistory::addToHistory(Song* song) {
    if (count < capacity) {
        HistoryNode* newNode = new HistoryNode(song);
        if (!head) {
            head = newNode;
            head->next = head; // circular
        }
        else {
            HistoryNode* tail = head;
            for (int i = 0; i < count - 1; i++) {
                tail = tail->next;
            }
            tail->next = newNode;
            newNode->next = head;
        }
        count++;
    }
    else {
        head->song = song;
        head = head->next;
    }
}

void PlayHistory::printHistory() {
    if (!head) return;
    HistoryNode* current = head;
    for (int i = 0; i < count; i++) {
        std::cout << current->song->title << " by " << current->song->artist << std::endl;
        current = current->next;
    }
}

bool PlayHistory::wasRecentlyPlayed(Song* song, int lookback) {
    if (!head) return false;
    HistoryNode* current = head;
    lookback = std::min(lookback, count);
    for (int i = 0; i < lookback; i++) {
        if (current->song->id == song->id) return true;
        current = current->next;
    }
    return false;
}

PlayHistory::~PlayHistory() {
    if (!head) return;
    HistoryNode* current = head->next;
    for (int i = 1; i < count; i++) {
        HistoryNode* temp = current;
        current = current->next;
        delete temp;
    }
    delete head;
}

// Playlist generation implementation

void buildBreadthFirstPlaylist(PlayQueue& queue,
    const std::vector<std::vector<Song*>>& tagResults, PlayHistory& history) {
    size_t maxSize = 0;
    for (const auto& result : tagResults) {
        maxSize = std::max(maxSize, result.size());
    }

    for (size_t i = 0; i < maxSize; i++) {
        for (size_t j = 0; j < tagResults.size(); j++) {
            if (i < tagResults[j].size() &&
                !history.wasRecentlyPlayed(tagResults[j][i])) {
                queue.enqueue(tagResults[j][i]);
            }
        }
    }
}

void buildWeightedShufflePlaylist(PlayQueue& queue,
    const std::vector<std::vector<Song*>>& tagResults, PlayHistory& history) {

    std::vector<std::pair<Song*, float>> weightedSongs;

    // Collect all unique songs from tag results
    for (const auto& tagGroup : tagResults) {
        for (Song* song : tagGroup) {
            // Check if song already in weightedSongs
            bool exists = false;
            for (const auto& pair : weightedSongs) {
                if (pair.first->id == song->id) {
                    exists = true;
                    break;
                }
            }

            if (!exists) {
                float weight = 1.0;
                // Reduce weight if song was recently played
                if (history.wasRecentlyPlayed(song, 5)) {
                    weight *= 0.3;  // 30% of original weight if recently played
                }

                // Potentially add other weight factors (like matching multiple tags)
                weightedSongs.push_back({ song, weight });
            }
        }
    }

    // Sort by weight in descending order (higher weights first)
    std::sort(weightedSongs.begin(), weightedSongs.end(),
        [](const auto& a, const auto& b) {
            return a.second > b.second;
        });

    // Enqueue songs based on weights
    for (const auto& pair : weightedSongs) {
        queue.enqueue(pair.first);
    }
}

// File I/O implementation

std::vector<Song*> loadSongLibrary(const std::string& filename) {
    std::vector<Song*> library;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return library;
    }

    std::string line;
    int id = 0;
    while (std::getline(file, line)) {
        // Parse CSV format (title,artist,genre,mood)
        std::string title, artist, genre, mood;
        size_t pos = 0;

        // Parse title
        pos = line.find(',');
        if (pos == std::string::npos) continue;
        title = line.substr(0, pos);
        line.erase(0, pos + 1);

        // Parse artist
        pos = line.find(',');
        if (pos == std::string::npos) continue;
        artist = line.substr(0, pos);
        line.erase(0, pos + 1);

        // Parse genre
        pos = line.find(',');
        if (pos == std::string::npos) continue;
        genre = line.substr(0, pos);
        line.erase(0, pos + 1);

        // Parse mood (rest of line)
        mood = line;

        // Create Song object and add to library
        Song* newSong = new Song(id++, title, artist, genre, mood);
        library.push_back(newSong);
    }

    return library;
}

void savePlaylist(PlayQueue& queue, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    // Store songs temporarily
    std::vector<Song*> songs;

    // Extract songs, maintain order
    while (!queue.isEmpty()) {
        songs.push_back(queue.dequeue());
    }

    // Write to file
    for (Song* song : songs) {
        file << song->title << "," << song->artist << ","
            << song->genre << "," << song->mood << std::endl;
    }

    // Restore queue
    for (Song* song : songs) {
        queue.enqueue(song);
    }



    file.close();
}

Song* PlayHistory::removeLastPlayed() {
    if (!head || count == 0) return nullptr;

    // If there's only one song in history
    if (count == 1) {
        Song* removed = head->song;
        delete head;
        head = nullptr;
        count = 0;
        return removed;
    }

    // Find the newest entry (the one that points back to head)
    HistoryNode* current = head;
    while (current->next != head) {
        current = current->next;
    }

    // Now, current points to the newest song
    Song* removed = current->song;

    // Find the node before current
    HistoryNode* beforeCurrent = head;
    while (beforeCurrent->next != current) {
        beforeCurrent = beforeCurrent->next;
    }

    // Remove current
    beforeCurrent->next = head;
    delete current;
    count--;

    return removed;
}