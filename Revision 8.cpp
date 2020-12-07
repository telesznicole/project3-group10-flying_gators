//Flying Gators
#include <chrono>
#include <iostream>
#include <vector>
#include <iterator>
#include <unordered_map>
#include <random>
#include <string>
#include <queue>
#include <fstream>
#include <stack>

using namespace std;

//Graph Class

class Graph {
private:
    vector<string> airports;
    int V;
    int E;
public:
    unordered_map<string, vector<pair<string, int>>> graph;

    //Constructor
    Graph(int numOfAiports, int numOfConnections, string bound) {
        graph = GenereateGraph(numOfAiports, numOfConnections, bound);
        V = numOfAiports;
        E = numOfConnections;
    };

    unordered_map<string, vector<pair<string, int>>> GenereateGraph(int numOfAirports, int numOfconnections, string bound);
    unordered_map<string, int> dijkstra(Graph& graph, string src, string dest);
    unordered_map<string, int> bellmanFord(Graph& graph, string src);
    unordered_map<string, vector<pair<string, int>>> GetGraph() {
        return graph;
    }

};

//Helper Functions
mt19937 generator{
        random_device{}()
};
string Increment(string curr) {
    string Incremented_string;
    char letter;

    //Case when string is _ZZ, returns (_+1)AA
    if (curr[2] == 'Z' && curr[1] == 'Z') {
        letter = curr[0];
        letter = static_cast<char>(letter + 1);
        Incremented_string.push_back(letter);
        Incremented_string.append("A");
        Incremented_string.append("A");
        return Incremented_string;
    }

    //Case when string is __Z, returns _(_+1)A
    else if (curr[2] == 'Z') {
        Incremented_string.push_back(curr[0]);
        letter = curr[1];
        letter = static_cast<char>(letter + 1);
        Incremented_string.push_back(letter);
        Incremented_string.append("A");
        return Incremented_string;
    }

    //case when last letter is not 'Z', returns __(_+1)
    else {
        Incremented_string.push_back(curr[0]);
        Incremented_string.push_back(curr[1]);
        letter = curr[2];
        letter = static_cast<char>(letter + 1);
        Incremented_string.push_back(letter);
        return Incremented_string;
    }
}
int GenerateRandomDistance() {
    //Generates a random num from 200-1086. Covers most flights in USA
    return (rand() / 30) + 200;
}
vector<pair<string, int>> GenerateRandomAirportConnections(string curr, string Bound, int NumConnections) {
    //Generates a random airport connection pair. Does not include itself and does not have parrallel loops.
    string Random_Connection;;
    int RandomDistance;
    vector<pair<string, int>> Connection;
    bool ParrallelEdge = false;

    while (Connection.size() < NumConnections) {
        uniform_int_distribution<int> distribution{ 'A', Bound[0] };
        auto generate_len = 1;
        string rand_str1(generate_len, '\0');
        for (auto& dis : rand_str1)
            dis = distribution(generator);

        uniform_int_distribution<int> distribution1{ 'A', Bound[1] };
        string rand_str2(generate_len, '\0');

        for (auto& dis : rand_str2)
            dis = distribution1(generator);

        uniform_int_distribution<int> distribution2{ 'A',  Bound[2] };
        string rand_str3(generate_len, '\0');

        for (auto& dis : rand_str3)
            dis = distribution2(generator);

        //Creates random Airport connection name
        Random_Connection = rand_str1 + rand_str2 + rand_str3;
        ParrallelEdge = false;
        for (int i = 0; i < Connection.size(); i++) {
            if (Connection.at(i).first == Random_Connection) {
                ParrallelEdge = true;
            }
        }

        //If the random connection isnt a parrellel edge, proceed, otherwise start over. Also check if this is a self loop
        if (ParrallelEdge == false && Random_Connection != curr) {
            RandomDistance = GenerateRandomDistance();
            Connection.push_back(pair<string, int>(Random_Connection, RandomDistance));
        }

    }
    return Connection;
}


//Graph creation
unordered_map<string, vector<pair<string, int>>> Graph::GenereateGraph(int numOfAirports, int numOfconnections, string bound) {
    string CurrentAirport = "AAA";
    vector<pair<string, int>> Connections;
    unordered_map<string, vector<pair<string, int>>> AirportGraph;

    for (int i = 0; i < numOfAirports; i++) {
        //Creates vector of random connections
        Connections.clear();
        Connections = GenerateRandomAirportConnections(CurrentAirport, bound, numOfconnections);


        //Creates Airport Node and pushes vector onto it
        AirportGraph.insert(pair<string, vector<pair<string, int>>>(CurrentAirport, Connections));

        //Increments airport IE AAA->AAB
        CurrentAirport = Increment(CurrentAirport);
    }

    return AirportGraph;
}

//SPF Algorithms
unordered_map<string, int> Graph::dijkstra(Graph& graph, string src, string dest) {
    priority_queue<pair<string, int>, vector<pair<string, int>>, greater<pair<string, int>>> pq;
    pq.push(make_pair(src, 0));
    unordered_map<string, int> dist;
    dist[src] = 0;
    unordered_map<string, string> previous;
    pair<string, int> curr;
    string u, v;
    int w;

    //initilize dist
    for (auto iter = graph.graph.begin(); iter != graph.graph.end(); iter++)
    {
        if (iter->first != src)
        {
            dist[iter->first] = INT_MAX;
        }
    }

    while (!pq.empty())
    {
        curr = pq.top();
        pq.pop();
        u = curr.first;

        //relax
        for (auto iter = graph.graph[curr.first].begin(); iter != graph.graph[curr.first].end(); iter++)
        {
            v = iter->first;
            w = iter->second;
            if (dist[u] + w < dist[v])
            {
                dist[v] = dist[u] + w;
                previous[v] = u;
                pq.push(make_pair(v, dist[v]));
            }
        }
    }

    //determine if connection between airports
    for (auto itr = dist.begin(); itr != dist.end(); itr++) {
        if ((*itr).first == dest) {
            //connection is found so find connecting flight(s)
            if ((*itr).second != 2147483647) {
                cout << endl << "Connecting flight(s) of total " << (*itr).second << " miles:\n";
                //determine path using prev
                stack<string> connectingFlights;
                string target = dest;
                while (previous.find(target) != previous.end()) {
                    connectingFlights.push(target);
                    target = previous[target];
                }
                cout << src << "->";
                while (!connectingFlights.empty()) {
                    //connectingFlights.pop();
                    cout << connectingFlights.top();
                    if (connectingFlights.size() != 1)
                        cout << "->";
                    connectingFlights.pop();
                }
                cout << endl;
            }
            //connection is not found
            else
                cout << "No connecting flights available\n";
        }
    }

    return dist;
}
unordered_map<string, int> Graph::bellmanFord(Graph& graph, string src)
{
    //bellman ford's O(|E|(|V|-1)) -> O(n^2) --for complete graph--
    queue<string>  q;
    q.push(src);
    unordered_map<string, int> dist;
    string u, v;
    dist[src] = 0;

    //initilize dist
    for (auto iter = graph.graph.begin(); iter != graph.graph.end(); iter++)
    {
        if (iter->first != src)
        {
            dist[iter->first] = INT_MAX;
        }
    }

    //relax v - 1 times
    for (int i = 0; i < graph.V - 1; i++)
    {
        while (!q.empty())
        {
            u = q.front();
            q.pop();
            for (auto itr = graph.graph[u].begin(); itr != graph.graph[u].end(); itr++)
            {
                v = itr->first;

                if (dist[u] + itr->second < dist[v])
                {
                    dist[v] = dist[u] + itr->second;
                    q.push(v);
                }
            }
        }
    }

    return dist;
}

//Printing
void PrintGraph(unordered_map<string, vector<pair<string, int>>> graph) {
    unordered_map<string, vector<pair<string, int>>>::iterator itr = graph.begin();
    for (int i = 0; i < graph.size(); i++) {
        cout << "Airport: " << itr->first << endl;
        for (int j = 0; j < itr->second.size(); j++) {
            cout << "Outbound flight to: " << itr->second.at(j).first << "  distance: " << itr->second.at(j).second << endl;
        }
        itr++;
        cout << endl;

    }
}
void PrintDijkstra(unordered_map<string, int> dist) {
    cout << endl << "Dijkstra results:" << endl;
    for (auto iter = dist.begin(); iter != dist.end(); iter++)
    {
        if (iter->second != 2147483647)
            cout << "airport: " << iter->first << " \ndistance: " << iter->second << endl << endl;
        else
            cout << "airport: " << iter->first << " \ndistance: " << "NO CONNECTIONS" << endl << endl;
    }
    cout << endl;

}
void PrintBellman(unordered_map<string, int> dist) {
    cout << endl << "Bellman Ford results:" << endl;
    for (auto iter = dist.begin(); iter != dist.end(); iter++)
    {
        if (iter->second != 2147483647)
            cout << "airport: " << iter->first << " \ndistance: " << iter->second << endl << endl;
        else
            cout << "airport: " << iter->first << " \ndistance: " << "NO CONNECTIONS" << endl << endl;
    }
    cout << endl;
}

//Writing
void WriteCSV(unordered_map<string, vector<pair<string, int>>> graph) {
    ofstream myfile("FlyingGatorsDatabase.csv");
    unordered_map<string, vector<pair<string, int>>>::iterator itr = graph.begin();

    myfile << "Source Aiport" << "," << "Destination Airport" << "," << "Distance" << endl;

    for (int i = 0; i < graph.size(); i++) {
        for (int j = 0; j < itr->second.size(); j++) {
            myfile << itr->first << "," << itr->second.at(j).first << "," << itr->second.at(j).second << endl;
        }
        itr++;
    }

}
void WriteDijkstra(unordered_map<string, int> dist) {
    ofstream myfile("Dijkstra.csv");
    unordered_map<string, int>::iterator itr = dist.begin();

    myfile << endl << "Destination" << "," << "Distance from source" << "," << endl;
    for (auto iter = dist.begin(); iter != dist.end(); iter++)
    {
        if (iter->second != 2147483647)
            myfile << iter->first << "," << iter->second << endl;
        else
            myfile << iter->first << "," << "NO CONNECTIONS" << endl;
    }
}
void WriteBellman(unordered_map<string, int> dist) {
    ofstream myfile("Bellman.csv");
    unordered_map<string, int>::iterator itr = dist.begin();

    myfile << endl << "Destination" << "," << "Distance from source" << "," << endl;
    for (auto iter = dist.begin(); iter != dist.end(); iter++)
    {
        if (iter->second != 2147483647)
            myfile << iter->first << "," << iter->second << endl;
        else
            myfile << iter->first << "," << "NO CONNECTIONS" << endl;
    }
}

//Program Menu
void Menu() {
    cout << "Please enter one of the following options:\n";
    cout << "0. Print out airport Graph\n";
    cout << "1. Search for an Airport\n";
    cout << "2. Search for a flight\n";
    cout << "3. Preform Dijkstra's\n";
    cout << "4. Preform Bellman Ford's\n";
    cout << "5. Export Dijkstra Results to \"Dijkstra.csv\"" << endl;
    cout << "6. Export Bellman Results to \"Bellman.csv\"" << endl;
    cout << "7. Export graph to \"FlyingGatorsDatabase.csv\" \n";
    cout << "-1: End program\n";
}

int main()
{
    //Variables
    int NumAirports, NumOutboundPerAirport;
    string bound, AirportToFind, AirportStart, AirportDestination, src;
    unordered_map<string, vector<pair<string, int>>>::iterator itr;

    cout << "Enter how many airports will be in the graph: ";
    cin >> NumAirports;
    cout << "Enter how many outbound connections each airport will have: ";
    cin >> NumOutboundPerAirport;
    cout << "Enter the string boundary (IE \"AAZ\"): ";
    cin >> bound;

    auto start = chrono::high_resolution_clock::now();
    Graph FlyingGators(NumAirports, NumOutboundPerAirport, bound);
    auto stop = chrono::high_resolution_clock::now();
    auto d_time = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << endl << "Graph creation took: " << d_time.count() << " microseconds" << endl;
    cout << "Graph creation took: " << d_time.count() / 1000000 << " seconds" << endl << endl;

    Menu();
    int option;
    cout << "Input: ";
    cin >> option;

    while (option != -1) {

        if (option == 0) {
            cout << endl;
            PrintGraph(FlyingGators.graph);
        }
        else if (option == 1) {
            //SEARCH AIRPORT
            cout << endl << "Enter the 3 letter code for the Airport: ";
            cin >> AirportToFind;
            itr = FlyingGators.graph.find(AirportToFind);
            if (itr != FlyingGators.graph.end()) {
                cout << "Airport: " << itr->first << endl << "Connections:" << endl;
                for (int i = 0; i < itr->second.size(); i++) {
                    cout << itr->second.at(i).first << "  " << itr->second.at(i).second << endl;
                }
                cout << endl;
            }
            else {
                cout << "Airport does not exist\n" << endl;
            }

        }
        else if (option == 2) {
            //SEARCH FLIGHT
            cout << endl << "Enter the departure airport: ";
            cin >> AirportStart;
            cout << "Enter the destination airport: ";
            cin >> AirportDestination;

            //if invalid airports given
            if (FlyingGators.graph.find(AirportStart) == FlyingGators.graph.end() ||
                FlyingGators.graph.find(AirportDestination) == FlyingGators.graph.end()) {
                cout << "No connecting flights available\n";
            }

            //else report shortest path between airports
            else {
                //use Dijkstra's algorithm to find all shortest paths, get time
                auto start_d = chrono::high_resolution_clock::now();
                FlyingGators.dijkstra(FlyingGators, AirportStart, AirportDestination);
                auto stop_d = chrono::high_resolution_clock::now();
                auto d_time = chrono::duration_cast<chrono::microseconds>(stop_d - start_d);

                //use the Bellman-Ford algorithm to find all shortest paths, get time
                auto start_bf = chrono::high_resolution_clock::now();
                FlyingGators.bellmanFord(FlyingGators, AirportDestination);
                auto stop_bf = chrono::high_resolution_clock::now();
                auto bf_time = chrono::duration_cast<chrono::microseconds>(stop_bf - start_bf);

                //report shortest connection and times
                //FlyingGators.dijkstra(FlyingGators, AirportStart,AirportDestination);
                cout << "Dijkstra's algorithm took: " << d_time.count() << " microseconds" << endl;
                cout << "The Bellman-Ford algorithm took: " << bf_time.count() << " microseconds" << endl;
                if (d_time > bf_time)
                    cout << "The Bellman-Ford algorithm is faster by " << (d_time.count() - bf_time.count()) << " microseconds.\n\n";
                else
                    cout << "Dijkstra's algorithm is faster by " << (bf_time.count() - d_time.count()) << " microseconds.\n\n";
            }
            cout << endl;

        }
        else if (option == 3)
        {
            //Perform Dijkstra
            cout << endl << "Enter the source to perform computation on: ";
            cin >> src;
            auto start = chrono::high_resolution_clock::now();
            PrintDijkstra(FlyingGators.dijkstra(FlyingGators, src, "000"));
            auto stop = chrono::high_resolution_clock::now();
            auto d_time = chrono::duration_cast<chrono::microseconds>(stop - start);
            cout << "Dijkstra's algorithm took: " << d_time.count() << " microseconds (" << d_time.count() / 1000000 << " seconds)\n\n";

        }
        else if (option == 4)
        {
            //Perfrom Bellman
            cout << endl << "Enter the source to perform computation on: ";
            cin >> src;
            auto start = chrono::high_resolution_clock::now();
            PrintBellman(FlyingGators.bellmanFord(FlyingGators, src));
            auto stop = chrono::high_resolution_clock::now();
            auto bf_time = chrono::duration_cast<chrono::microseconds>(stop - start);
            cout << "The Bellman-Ford algorithm took: " << bf_time.count() << " microseconds (" << bf_time.count() / 1000000 << " seconds)\n\n";



        }
        else if (option == 5) {
            //Write Dijstra
            cout << endl << "Enter the source to perform computation on: ";
            cin >> src;
            WriteDijkstra(FlyingGators.dijkstra(FlyingGators, src, "000"));
            cout << "File succesfull exported" << endl << endl;
        }
        else if (option == 6) {
            //Write Bellman
            cout << endl << "Enter the source to perform computation on: ";
            cin >> src;
            WriteBellman(FlyingGators.bellmanFord(FlyingGators, src));
            cout << "File succesfull exported" << endl << endl;
        }
        else if (option == 7) {
            WriteCSV(FlyingGators.graph);
            cout << "File succesfull exported" << endl << endl;

        }
        else {
            cout << "\nInvalid input\n\n";
        }

        Menu();
        cout << "Input: ";
        cin >> option;
    }

    return 0;
}