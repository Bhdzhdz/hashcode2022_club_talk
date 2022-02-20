#include <iostream> 
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <filesystem>
#include <set>
#include <unordered_set>
#include <map>

using namespace std;

struct Client{
    vector<int> likes;
    vector<int> dislikes;
};

struct Solution{
    int score;
    unordered_set<int> liked_ingredients; // {1,2,3} agrega el elemento 4 -> {1,2,3,4}
    unordered_set<int> disliked_ingredients;
};

tuple<vector<Client>, map <int, string>> read_data(string filename) {
    ifstream file(filename);
    
    int n;

    //cin >> n;
    file >> n;

    vector<Client> clients(n);

    int next_id = 0;
    map <int, string> id_to_name;
    map <string, int> name_to_id;

    for (int i = 0; i < n; i++) {
        int likes, dislikes;

        file >> likes;

        for (int j = 0; j < likes; j++) {
            string like;
            file >> like;

            if(not name_to_id.count(like)) {
                name_to_id[like] = next_id;
                id_to_name[next_id] = like;
                next_id++;
            }

            clients[i].likes.push_back(name_to_id[like]);
        }

        file >> dislikes;

        for (int j = 0; j < dislikes; j++) {
            string dislike;
            file >> dislike;

            if(not name_to_id.count(dislike)) {
                name_to_id[dislike] = next_id;
                id_to_name[next_id] = dislike;
                next_id++;
            }

            clients[i].dislikes.push_back(name_to_id[dislike]);
        }

        sort(clients[i].likes.begin(), clients[i].likes.end());
        sort(clients[i].dislikes.begin(), clients[i].dislikes.end());
        
    }

    return {clients, id_to_name};
}

struct Counter
{
  struct value_type { template<typename T> value_type(const T&) { } };
  void push_back(const value_type&) { ++count; }
  size_t count = 0;
};

template<typename T1, typename T2>
size_t intersection_size(const T1& s1, const T2& s2)
{
  Counter c;
  set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), std::back_inserter(c));
  return c.count;
}

template<typename T1, typename T2>
size_t difference_size(const T1& s1, const T2& s2)
{
  Counter c;
  set_difference(s1.begin(), s1.end(), s2.begin(), s2.end(), std::back_inserter(c));
  return c.count;
}

template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly(start, end, gen);
}

// solver / scorer
Solution generate_randomized_greedy_solution(vector<Client> clients) {
    Solution solution;
    solution.score = 0;
    solution.liked_ingredients.clear();
    solution.disliked_ingredients.clear();

    int best_to_add;
    int best_added_dislikes;

    set<int> unvisited;

    random_device rd;
    mt19937 g(rd()); 
    shuffle(clients.begin(), clients.end(), g);

    for(int i = 0; i < clients.size(); i++){
        unvisited.insert(i);
    }

    int progress_bar_size = 50;
    int progress_bar_step;
    int progress_bar_step_counter = 0;
    int progress_bar_counter = 0;

    if(unvisited.size() > progress_bar_size){
        progress_bar_step = unvisited.size() / progress_bar_size;
    } else {
        progress_bar_step = 1;
    }

    cout << "[";
    cout.flush();

    while(unvisited.size()){
        

        // podemos agregar al cliente?
        vector<int> options_to_add;
        best_to_add = -1;
        best_added_dislikes = INT32_MAX;

        vector<int> to_visit(unvisited.begin(), unvisited.end());

        for(int i : to_visit) {

            
            auto client = clients[i];

            bool invalid = false;
            int added_dislikes = 0; 

            for(auto liked_ingredint : client.likes) {
                if(solution.disliked_ingredients.count(liked_ingredint)) {
                    invalid = true;
                    break;
                }
            }         

            if(invalid){
                unvisited.erase(i);
                progress_bar_counter++;
                if(progress_bar_counter == progress_bar_step){
                    progress_bar_counter = 0;
                    cout << "=";
                    cout.flush();
                }
                continue;
            }

            for(auto disliked_ingredint : client.dislikes) {
                if(solution.liked_ingredients.count(disliked_ingredint)) {
                    invalid = true;
                    break;
                }
            }

            if(invalid){
                unvisited.erase(i);
                progress_bar_counter++;
                if(progress_bar_counter == progress_bar_step){
                    progress_bar_counter = 0;
                    cout << "=";
                    cout.flush();
                }
                continue;
            }

            added_dislikes = difference_size(client.dislikes, solution.disliked_ingredients);

            if(added_dislikes > best_added_dislikes){
                continue;
            }

            options_to_add.push_back(i);
            best_added_dislikes = added_dislikes;
        }


    
        if(options_to_add.size() <= 0){
            break;
        }


        auto begin_better_options = next(options_to_add.begin(), 3 * (options_to_add.size() / 4));

        best_to_add = *select_randomly(begin_better_options, options_to_add.end());

        unvisited.erase(best_to_add);
        progress_bar_counter++;
        if(progress_bar_counter == progress_bar_step){
            progress_bar_counter = 0;
            cout << "=";
            cout.flush();
        }

        solution.score = solution.score + 1;

        solution.liked_ingredients.insert(
            clients[best_to_add].likes.begin(),
            clients[best_to_add].likes.end()
        );
        solution.disliked_ingredients.insert(
            clients[best_to_add].dislikes.begin(),
            clients[best_to_add].dislikes.end()
        );
    }

    cout << "]" << endl;


    return solution;
}

void write_solution(string filename, Solution solution, map<int, string> id_to_name) {
    ofstream file(filename);

    file << solution.liked_ingredients.size() << ' ';

    for(auto ingredient : solution.liked_ingredients) {
        file << id_to_name[ingredient] << ' ';
    }

    file << '\n';
}



int main(){

    //const int repeat = 1000;

    for(auto& p: std::filesystem::directory_iterator("input_data")){
        cout << p.path().generic_string() << endl;

        auto [clients, id_ingredients] = read_data(p.path().generic_string());
        //auto clients = read_data("input_data/c_coarse.in.txt");

        Solution best_solution;
        best_solution.score = 0;
        
        for(int i = 0; i < 10; i++){
            cout << "Generating solution " << i+1 << endl;

            Solution solution = generate_randomized_greedy_solution(clients);

            if(solution.score > best_solution.score)
                best_solution = solution;
        }

        cout << "Best solution: " << best_solution.score << endl;

        string output_filename = (
            "randomized_greedy/output_data/" 
            + p.path().stem().stem().generic_string()
            + ".out.txt"
        );

        write_solution(output_filename, best_solution, id_ingredients);
    }
    return 0;
}