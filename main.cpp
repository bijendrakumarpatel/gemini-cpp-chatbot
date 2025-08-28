#include "Crow/include/crow.h"
#include "json/include/nlohmann/json.hpp"
#include <curl/curl.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <fstream>
#include <sstream>

using json = nlohmann::json;
using namespace std;



// IMPORTANT: Replace with your actual Gemini API key
const string API_KEY = "Y"; 
const string API_URL = "https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent";

// In-memory session store: session token to conversation history
unordered_map<string, json> sessions;
mutex sessions_mutex;

// Curl callback to accumulate response body
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Gemini API call with full conversation payload (multi-turn)
string gemini_generate(const json& conversation_history) {
    CURL* curl = curl_easy_init();
    string readBuffer;
    if (!curl) {
        cerr << "Failed to init curl" << endl;
        return "";
    }

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("x-goog-api-key: " + API_KEY).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    json payload = {
        {"contents", conversation_history}
    };

    string body = payload.dump();

    curl_easy_setopt(curl, CURLOPT_URL, API_URL.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        cerr << "Curl error: " << curl_easy_strerror(res) << endl;
        return "";
    }
    return readBuffer;
}

// Parse Gemini API JSON response
string extract_gemini_text(const string& json_response) {
    try {
        auto response = json::parse(json_response);
        if (response.contains("error")) {
            return "[ERROR] " + response["error"]["message"].get<string>();
        }
        if (response.contains("candidates") && !response["candidates"].empty()) {
            return response["candidates"][0]["content"]["parts"][0]["text"].get<string>();
        }
    } catch (const exception& e) {
        return string("[Error] JSON parsing failed: ") + e.what();
    }
    return "[No valid reply]";
}

// Function to read the contents of a file (like index.html)
std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "Error: Could not open file " + path;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


// --- Main Application ---

int main() {
    crow::SimpleApp app;

    // Route to serve the main HTML file
    CROW_ROUTE(app, "/")([](){
        string html_content = readFile("index.html");
        return crow::response(200, "text/html", html_content);
    });

    // API Route for handling chat messages
    CROW_ROUTE(app, "/api/chat").methods("POST"_method)
    ([](const crow::request& req){
        auto request_body = json::parse(req.body);

        if (!request_body.contains("message") || !request_body.contains("session")) {
            return crow::response(400, "Invalid JSON: 'message' and 'session' fields required.");
        }

        string user_message = request_body["message"];
        string session_token = request_body["session"];

        json conversation;
        
        { // Lock to safely access the shared sessions map
            lock_guard<mutex> lock(sessions_mutex);
            // Retrieve existing conversation or start a new one
            if (sessions.count(session_token)) {
                conversation = sessions[session_token];
            } else {
                conversation = json::array(); // Start a new history
            }
        }

        // Add the new user message to the conversation history
        conversation.push_back({{"role", "user"}, {"parts", {{{"text", user_message}}}}});

        string api_response_str = gemini_generate(conversation);
        string answer = extract_gemini_text(api_response_str);

        // Add the model's response to the conversation history
        conversation.push_back({{"role", "model"}, {"parts", {{{"text", answer}}}}});
        
        { // Lock again to update the session
            lock_guard<mutex> lock(sessions_mutex);
            sessions[session_token] = conversation;
        }

        json response_json;
        response_json["reply"] = answer;
        response_json["session"] = session_token; // Send the session token back

        return crow::response(200, response_json.dump());
    });

    cout << "Server starting on port 18080..." << endl;
    app.port(18080).multithreaded().run();

    return 0;
}
