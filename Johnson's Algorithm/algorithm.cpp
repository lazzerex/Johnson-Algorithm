#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include <queue>

using namespace std;

const int INF = numeric_limits<int>::max();

struct Edge {
    int to, weight;
};

class Graph {
public:
    Graph(int vertices) : adj(vertices) {}

    void addEdge(int from, int to, int weight) {
        adj[from].push_back({to, weight});
    }

    vector<vector<int>> johnson() {
        int n = adj.size();
        // Step 1: Add a new vertex
        vector<Edge> newEdges;
        for (int u = 0; u < n; ++u) {
            newEdges.push_back({n, 0}); // Connect new vertex to all existing vertices with 0 weight
        }
        adj.push_back(newEdges); // Add the new vertex

        // Step 2: Run Bellman-Ford to find h values
        vector<int> h(n + 1, 0); // h[n] is the new vertex
        for (int i = 0; i < n; ++i) {
            h[i] = INF; // Initialize h values
        }
        h[n] = 0; // h[n] = 0 for the new vertex

        for (int i = 0; i < n; ++i) {
            for (int u = 0; u <= n; ++u) {
                for (const Edge &edge : adj[u]) {
                    if (h[u] != INF && h[edge.to] > h[u] + edge.weight) {
                        h[edge.to] = h[u] + edge.weight;
                    }
                }
            }
        }

        // Check for negative-weight cycles
        for (int u = 0; u <= n; ++u) {
            for (const Edge &edge : adj[u]) {
                if (h[u] != INF && h[edge.to] > h[u] + edge.weight) {
                    throw runtime_error("Graph contains a negative-weight cycle");
                }
            }
        }

        // Step 3: Reweight edges
        vector<vector<int>> distance(n, vector<int>(n, INF));
        for (int u = 0; u < n; ++u) {
            for (const Edge &edge : adj[u]) {
                distance[u][edge.to] = edge.weight + h[u] - h[edge.to];
            }
        }

        // Step 4: Run Dijkstra's algorithm for each vertex
        for (int u = 0; u < n; ++u) {
            vector<int> dist(n, INF);
            dist[u] = 0;
            priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
            pq.push({0, u});

            while (!pq.empty()) {
                auto [d, v] = pq.top();
                pq.pop();

                if (d > dist[v]) continue;

                for (int to = 0; to < n; ++to) {
                    if (distance[v][to] < INF) {
                        int newDist = dist[v] + distance[v][to];
                        if (newDist < dist[to]) {
                            dist[to] = newDist;
                            pq.push({newDist, to});
                        }
                    }
                }
            }

            // Reweight distances
            for (int v = 0; v < n; ++v) {
                if (dist[v] < INF) {
                    dist[v] += h[v] - h[u];
                }
                distance[u][v] = dist[v];
            }
        }

        // Remove the new vertex
        adj.pop_back();

        return distance;
    }

private:
    vector<vector<Edge>> adj;
};

int main() {
    Graph g(5); // Create a graph with 5 vertices
    g.addEdge(0, 1, -2);
    g.addEdge(1, 2, 3);
    g.addEdge(2, 0, 2);
    g.addEdge(0, 3, 4);
    g.addEdge(3, 4, 5);
    g.addEdge(4, 2, -1);

    try {
        vector<vector<int>> distances = g.johnson();
        for (int i = 0; i < distances.size(); ++i) {
             for (int j = 0; j < distances[i].size(); ++j) {
                if (distances[i][j] == INF) {
                    cout << "INF ";
                } else {
                    cout << distances[i][j] << " ";
                }
            }
            cout << endl;
        }
    } catch (const runtime_error& e) {
        cout << e.what() << endl;
    }

    return 0;
}
