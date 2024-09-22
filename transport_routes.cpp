#include <iostream>
#include <vector>
#include <map>
#include <cmath>
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
    Graph cityGraph;

    // Створюємо список міст
    vector<City> cities;
    int CityId = 9;

    // Додаємо 10 найбільших міст України з ID та координатами
    cities.push_back(City(0, "Kyiv", Position(0, 0, 0)));      // Умовні координати
    cities.push_back(City(1, "Kharkiv", Position(500, 150, 0)));   // Координати умовні, у км
    cities.push_back(City(2, "Odessa", Position(0, 475, 0)));
    cities.push_back(City(3, "Dnipro", Position(275, -150, 0)));
    cities.push_back(City(4, "Donezk", Position(600, -50, 0)));
    cities.push_back(City(5, "Zaporizhzhia", Position(350, -250, 0)));
    cities.push_back(City(6, "Lviv", Position(-500, 100, 0)));
    cities.push_back(City(7, "Kryvyi Rih", Position(200, -350, 0)));
    cities.push_back(City(8, "Mykolaiv", Position(100, -450, 0)));
    cities.push_back(City(9, "Mariupol", Position(600, -250, 0)));

    //меню
    int value;

    while (value)
    {
        cout << "\nEnter: "<<endl;
        cout << "0 - exit" << endl;
        cout << "1 - add a city" << endl;
        cout << "2 - choose a route" << endl;
        switch (value)
        {
            case 0: return 0;
            case 1:{
                CityId++;
                addCity(cities, CityId);
                break;
            }
            case 2: {

            }
        }
    }

    // Додаємо міста як вершини графа
    for (const auto& city : cities) {
        Position pos = city.getPosition();
        cityGraph.addNode(city.getId(), pos.X, pos.Y, pos.Z);
    }

    // Додаємо деякі ребра між містами, де вага дорівнює відстані
    cityGraph.addEdge(0, 1);  
    cityGraph.addEdge(0, 2);  
    cityGraph.addEdge(1, 3);  
    cityGraph.addEdge(2, 4);  
    cityGraph.addEdge(4, 3);  

    // Виводимо граф
    cityGraph.printGraph();

    return 0;
}