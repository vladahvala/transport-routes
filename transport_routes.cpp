#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <curl/curl.h>
#include <json/json.h>
using namespace std;

class Position {
public:
    double X;
    double Y;
    double Z;
    Position(double x_, double y_, double z_) : X(x_), Y(y_), Z(z_) {}
};

class City {
private:
    int id;              // ID міста
    Position position;
    string name;

public:
    City(int city_id, string city_name, Position pos) : id(city_id), name(city_name), position(pos) {}

    int getId() const { return id; }        // Отримати ID міста
    string getName() const { return name; }
    Position getPosition() const { return position; }
};

// Функція для виконання запитів через libcurl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* s) {
    size_t newLength = size * nmemb;
    s->append((char*)contents, newLength);
    return newLength;
}

// Функція для отримання географічних координат за допомогою Nominatim API (OSM)
string getCoordinatesFromOSM(const string& city) {
    CURL* curl;
    CURLcode res;
    string readBuffer;

    string url = "https://nominatim.openstreetmap.org/search?q=" + city + "&format=json&limit=1";

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    return readBuffer;  // Повертаємо результат у вигляді JSON-строки
}

// Функція для парсингу координат з JSON-відповіді
Position parseCoordinatesFromJSON(const string& jsonString) {
    Json::Value jsonData;
    Json::Reader jsonReader;

    if (jsonReader.parse(jsonString, jsonData) && !jsonData.empty()) {
        double lat = stod(jsonData[0]["lat"].asString());
        double lon = stod(jsonData[0]["lon"].asString());
        return Position(lon, lat, 0);  // Координати (X = довгота, Y = широта)
    }

    return Position(0, 0, 0);  // Якщо не вдалось знайти координати
}

// Функція для обчислення відстані між двома точками (широта і довгота) за формулою Haversine
double haversine(double lat1, double lon1, double lat2, double lon2) {
    const double EARTH_RADIUS = 6371.0;  // Радіус Землі в км
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;

    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;

    double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(lat1) * cos(lat2) * sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return EARTH_RADIUS * c;  // Відстань у кілометрах
}

class Graph {

private:

    class Node {
    public:
        int id;
        double x, y, z;
        Node(int id_, double x_, double y_, double z_) : id(id_), x(x_), y(y_), z(z_) {}
    };

    class Edge {
    public:
        Node* from;
        Node* to;
        double weight;
        Edge(Node* from_, Node* to_, double weight_) : from(from_), to(to_), weight(weight_) {}
    };

public:
    map<int, Node*> nodes;
    vector<Edge> edges;

    void addNode(int id, double x, double y, double z) {
        nodes[id] = new Node(id, x, y, z);
    }

    // Функція для обчислення відстані між двома точками
    double calculateDistance(Node* from, Node* to) {
        return sqrt(pow(to->x - from->x, 2) + pow(to->y - from->y, 2) + pow(to->z - from->z, 2));
    }

    // Додаємо ребро, де вага рівна відстані між містами
    void addEdge(int fromId, int toId) {
        Node* from = nodes[fromId];
        Node* to = nodes[toId];
        double distance = calculateDistance(from, to);
        edges.push_back(Edge(from, to, distance));
    }

    void printGraph() {
        for (const auto& edge : edges) {
            cout << "Edge from Node " << edge.from->id
                << " to Node " << edge.to->id
                << " with distance " << edge.weight << " km" << endl;
        }
    }

    ~Graph() {
        for (auto& node : nodes) {
            delete node.second;
        }
    }
};

// Базовий клас транспортного засобу
class Vehicle {
protected:
    double speed;
    Position position;

public:
    Vehicle(double s, Position pos) : speed(s), position(pos) {}

    virtual void move() = 0;

    Position getPosition() const { return position; }

    void setPosition(const Position& newPos) {
        position = newPos;
    }

    friend class Environment;
};

class LandVehicle : public Vehicle {
public:
    LandVehicle(double s, Position pos) : Vehicle(s, pos) {}

    void move() override {
        position.X += speed;
        position.Y += speed;
        cout << "Land vehicle is moving to X: " << position.X << " Y: " << position.Y << endl;
    }
};

class WaterVehicle : public Vehicle {
public:
    WaterVehicle(double s, Position pos) : Vehicle(s, pos) {}

    void move() override {
        position.X += speed;
        position.Y += speed;
        cout << "Water vehicle is moving to X: " << position.X << " Y: " << position.Y << endl;
    }
};

class AirVehicle : public Vehicle {
public:
    AirVehicle(double s, Position pos) : Vehicle(s, pos) {}

    void move() override {
        position.X += speed;
        position.Y += speed;
        position.Z += speed;
        cout << "Air vehicle is moving to X: " << position.X << " Y: " << position.Y << " Z: " << position.Z << endl;
    }
};

class Environment {
protected:
    Position departure_point;
    Position destination;

public:
    Environment(Position dep, Position dest) : departure_point(dep), destination(dest) {}

    friend class Vehicle;
    friend class Simulation;
};

class Land : public Environment {
public:
    Land(Position dep, Position dest) : Environment(dep, dest) {}
};

class Water : public Environment {
public:
    Water(Position dep, Position dest) : Environment(dep, dest) {}
};

class Air : public Environment {
public:
    Air(Position dep, Position dest) : Environment(dep, dest) {}
};

class Simulation {
public:
    void simulateMovement(Vehicle& vehicle, Environment& env) {
        vehicle.setPosition(env.departure_point);
        cout << "Starting simulation from X: " << env.departure_point.X
            << " Y: " << env.departure_point.Y
            << " Z: " << env.departure_point.Z << endl;
        vehicle.move();
    }
};

// Функція для додавання нового міста у вектор
void addCity(vector<City>& cities, int cityId) {
    string cityName;
    double x, y;
    cout << "Enter city name: ";
    cin >> cityName;
    cout << "\nEnter city coordinates\nX: ";
    cin >> x;
    cout << "\nY: ";
    cin >> y;
    cities.push_back(City(cityId, cityName, Position(x, y, 0)));
}

int main() {
    string city1, city2;

    // Запитуємо у користувача назви двох міст
    cout << "Enter the first city name: ";
    cin >> city1;
    cout << "Enter the second city name: ";
    cin >> city2;

    // Отримуємо географічні координати для обох міст
    string response1 = getCoordinatesFromOSM(city1);
    string response2 = getCoordinatesFromOSM(city2);

    // Парсимо координати
    Position pos1 = parseCoordinatesFromJSON(response1);
    Position pos2 = parseCoordinatesFromJSON(response2);

    if (pos1.X == 0 && pos1.Y == 0 || pos2.X == 0 && pos2.Y == 0) {
        cout << "Failed to retrieve coordinates for one or both cities." << endl;
        return 1;
    }

    cout << "Coordinates of " << city1 << ": Longitude = " << pos1.X << ", Latitude = " << pos1.Y << endl;
    cout << "Coordinates of " << city2 << ": Longitude = " << pos2.X << ", Latitude = " << pos2.Y << endl;

    // Обчислюємо відстань між двома містами
    double distance = haversine(pos1.Y, pos1.X, pos2.Y, pos2.X);

    cout << "Distance between " << city1 << " and " << city2 << " is: " << distance << " km" << endl;

    return 0;

}