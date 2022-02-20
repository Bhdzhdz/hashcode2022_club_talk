#include <iostream> 
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <filesystem>
#include <set>


using namespace std;

struct Client{
    vector<string> likes;
    vector<string> dislikes;
};

struct Solution{
    int score;
    set<string> liked_ingredients; // {1,2,3} agrega el elemento 4 -> {1,2,3,4}
    set<string> disliked_ingredients;
};



vector<Client> read_data(string filename) {
    ifstream file(filename);
    
    int n;

    //cin >> n;
    file >> n;

    vector<Client> clients(n);

    for (int i = 0; i < n; i++) {
        int likes, dislikes;

        file >> likes;

        for (int j = 0; j < likes; j++) {
            string like;
            file >> like;
            clients[i].likes.push_back(like);
        }

        file >> dislikes;

        for (int j = 0; j < dislikes; j++) {
            string dislike;
            file >> dislike;
            clients[i].dislikes.push_back(dislike);
        }
    }

    return clients;
}

// solver / scorer
Solution generate_random_solution(vector<Client> clients) {
    Solution solution;
    solution.score = 0;
    solution.liked_ingredients.clear();
    solution.disliked_ingredients.clear();

    random_device rd;
    mt19937 g(rd()); 
    shuffle(clients.begin(), clients.end(), g);

    for(auto client : clients) {
        bool invalid = false;

        for(string liked_ingredient : client.likes){
            if(solution.disliked_ingredients.count(liked_ingredient)){
                invalid = true;
                break;
            }
        }

        if(invalid){
            continue;
        }

        for(string disliked_ingredient : client.dislikes){
            if(solution.liked_ingredients.count(disliked_ingredient)){
                invalid = true;
                break;
            }
        }

        if(invalid){
            continue;
        }

        solution.score = solution.score + 1;

        solution.liked_ingredients.insert(client.likes.begin(), client.likes.end());
        solution.disliked_ingredients.insert(client.dislikes.begin(), client.dislikes.end());
    }

    return solution;
}

void write_solution(string filename, Solution solution) {
    ofstream file(filename);

    file << solution.liked_ingredients.size() << ' ';

    for(auto ingredient : solution.liked_ingredients) {
        file << ingredient << ' ';
    }

    file << '\n';
}



int main(){

    const int repeat = 100;

    for(auto& p: std::filesystem::directory_iterator("input_data")){
        cout << p.path().generic_string() << endl;

        auto clients = read_data(p.path().generic_string());
        //auto clients = read_data("input_data/c_coarse.in.txt");

        Solution best_solution;
        best_solution.score = 0;

        for(int i = 0; i < repeat; i++){
            Solution solution = generate_random_solution(clients);

            if(solution.score > best_solution.score)
                best_solution = solution;
        }

        cout << "Best solution: " << best_solution.score << endl;

        string output_filename = (
            "random_solver/output_data/" 
            + p.path().stem().stem().generic_string()
            + ".out.txt"
        );

        write_solution(output_filename, best_solution);
    }
    return 0;
}